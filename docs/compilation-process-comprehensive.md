# AstroScript Compilation Process: High-Level to Deep-Dive

This document explains the complete AstroScript compilation flow from source code to execution, then traces exactly how the frontend playground triggers and presents that flow.

It is grounded in the current implementation in:
- backend/compiler/main.cpp
- backend/compiler/lexer/lexer.l
- backend/compiler/parser/parser.y
- backend/compiler/semantic/symbol_table.h
- backend/compiler/semantic/symbol_table.cpp
- backend/compiler/ir/tac.h
- backend/compiler/ir/tac.cpp
- web/src/app/api/compile/route.ts
- web/src/app/api/health/route.ts
- web/src/lib/compilerPath.ts
- web/src/app/playground/page.tsx
- web/src/components/playground/CodeEditorPanel.tsx
- web/src/components/playground/OutputPanel.tsx
- web/scripts/generate-language-reference.mjs

---

## 1) High-Level Overview

AstroScript uses a classic educational compiler pipeline:

1. Lexing (Flex): source text -> token stream.
2. Parsing + semantic actions (Bison): token stream -> grammar validation + TAC emission.
3. Symbol checks: declarations, duplicate names, undeclared usage checks.
4. IR generation: three-address code (TAC) instructions.
5. TAC optimization: constant folding, algebraic simplification, redundant move removal.
6. TAC execution: interpreter runs optimized TAC.
7. Output projection: printable lines (`PRINT:`), TAC dump, errors.

Frontend playground integration:

1. User writes code in Monaco editor.
2. Frontend calls `/api/compile`.
3. API writes temporary `.as` file, executes compiler binary.
4. API parses compiler stdout/stderr into `output`, `ir`, `tokens`, `diagnostics`.
5. UI displays runtime output, intermediate code, and clickable diagnostics.

---

## 2) Backend Compilation Pipeline (Detailed)

## 2.1 Entry Point and Global Flow (`main.cpp`)

`main.cpp` is the orchestrator:

1. Prints startup banner.
2. Opens source file from CLI argument and assigns to `yyin` for lexer input.
3. Calls `yyparse()`.
4. If parse returns `0` (success):
   - `tacGenerator.optimize()`
   - `tacGenerator.printCode("Optimized Three Address Code")`
   - `tacGenerator.execute()`
   - `symbolTable.printTable()`
5. Returns parse status.

Important behavior:
- Semantic errors are currently printed during parse actions but do not hard-stop parse automatically.
- Runtime errors are emitted during TAC execution as `RUNTIME ERROR: ...`.

## 2.2 Lexical Analysis (`lexer/lexer.l`)

The lexer:

1. Matches mission keywords (`mission`, `launch`, `success`, ...).
2. Matches declarations/types (`telemetry`, `count`, `real`, ...).
3. Matches control flow (`verify`, `orbit`, `scenario`, ...).
4. Matches operators (`add`, `minus`, `mul`, `divide`, `mod`, `**`, relations, assignment).
5. Emits punctuation tokens (`{}`, `()`, `[]`, `.`, `,`, `:`).
6. Parses literals:
   - Integer (`INT_LITERAL`)
   - Float (`FLOAT_LITERAL`)
   - String (`STRING_LITERAL`)
7. Ignores comments:
   - Single-line: `$$ ...`
   - Multi-line: `$* ... *$`
8. Reports unknown characters as:
   - `LEXICAL ERROR at line N: ...`

Notes:
- `+ - * /` and keyword operators are normalized into token classes (`ADD`, `MINUS`, etc.).
- All lexer keywords now have parser productions and active runtime behavior.

## 2.3 Parsing + Semantic Actions (`parser/parser.y`)

The parser both validates syntax and emits TAC on-the-fly.

### Core grammar structure

- Program: `mission ... launch { ... } success`
- Statement categories include:
  - declaration
  - assignment
  - print/input
  - control flow
  - loops
  - switch-style scenarios
  - function/module definitions
  - wait
  - return
  - abort

### Declarations

Implemented forms include:
- scalar declaration
- scalar declaration with initialization
- limit declaration (constant-like but currently mutable at runtime)
- arrays (`telemetry type name[size].`)
- mode definitions with `trajectory`/`fallback` entries
- alias/fleet declarations are parsed but currently non-executing placeholders

### Assignment and array indexing

- Scalar assignment emits `=` TAC.
- Array assignment emits `store` TAC.
- Undeclared names produce semantic error prints.

### Expressions

Supported expression families:
- arithmetic: `+ - * / % **`
- relational: `< > <= >= == !=`
- logical: `AND OR XOR NOT`
- literals and identifiers
- array load expressions
- function call expressions

Semantic action highlights:
- Function arity check is now enforced when function signature is known:
  - `Semantic Error at line N: function f expects X arguments but got Y`

### Control flow

- `verify / else_verify / otherwise` emits labels + conditional gotos.
- `orbit (...)` and `orbit while (...)` emit loop labels.
- `orbit times (i : start : end)` emits assignment, condition, increment TAC.
- `scenario/trajectory/fallback` emits switch-like compare-and-jump TAC.

### Modules and functions

- Module syntax is parsed with optional `extends`.
- Members can include declarations and functions.
- Functions emit:
  - `func_begin`
  - `param_def` for parameters
  - body TAC
  - implicit or explicit `return`
  - `func_end`

## 2.4 Symbol Table (`semantic/symbol_table.*`)

Current symbol table responsibilities:
- Insert/check symbols by name.
- Store declared type text.
- Store numeric value slot (lightweight).
- Report table at end of run.

Current semantic checks are name-centric (existence/duplicates), not full static type checking.

## 2.5 TAC Model (`ir/tac.h`)

`TACInstruction` fields:
- `op`
- `arg1`
- `arg2`
- `result`

Runtime data model:
- `RuntimeValue = variant<double, string>`
- Frame stack (`variables` map per frame)
- Global array storage map
- Function boundary metadata
- Parameter stack for function calls

## 2.6 TAC Emission and Execution (`ir/tac.cpp`)

### Emission API

Key emitters:
- declarations: `decl`, `decl_arr`
- movement: `=`
- arithmetic/comparison/logical via `emitBinary`
- control: `label`, `goto`, `ifFalse`
- I/O: `print`, `input`
- arrays: `store`, `load`
- functions: `func_begin`, `param_def`, `param`, `call`, `return`, `func_end`

### Optimization passes

1. Constant folding
   - Folds numeric binary operations when safe.
   - Division/modulo by zero are not folded away.
2. Algebraic simplification
   - Examples: `x+0 -> x`, `x*1 -> x`, `x*0 -> 0`.
3. Redundant move elimination
   - Removes `x = x`.

### Interpreter execution loop

Execution handles each TAC op with a program counter.

Now includes runtime diagnostics for critical failures:
- division by zero
- modulo by zero
- bad array reference (undeclared array)
- array index out of bounds
- invalid jump target
- invalid conditional jump target
- undefined function call
- stack overflow guard (`kMaxCallDepth = 1024`)
- invalid frame states on return/end
- out-of-memory during array allocation
- negative array size (clamped with error)

Runtime error format:
- `RUNTIME ERROR: <message>`

This format is consumed by frontend diagnostics parsing.

---

## 3) Frontend Playground Connection (Detailed)

## 3.1 Compiler discovery (`web/src/lib/compilerPath.ts`)

`resolveCompilerPath()` tries, in order:
1. `ASTROSCRIPT_COMPILER_PATH` env var.
2. `../backend/compiler/build/astroscript.exe`
3. `../backend/compiler/build/astroscript`

This keeps playground portable across Windows/Linux/macOS builds.

## 3.2 Health endpoint (`/api/health`)

`GET /api/health`:
- Resolves compiler path.
- Returns `compilerReady: true/false` with metadata.
- Used by UI status badges and run guard.

## 3.3 Compile endpoint (`/api/compile`)

`POST /api/compile` flow:

1. Validate request JSON with `code` string.
2. Resolve compiler path; fail with actionable message if missing.
3. Write code to temporary `.as` file under OS tmp dir.
4. Execute compiler with timeout and output buffer limit.
5. Parse stdout/stderr into UI payload:
   - `output`: lines prefixed `PRINT:`
   - `ir`: section under `--- Optimized Three Address Code ---`
   - `tokens`: token-related lines if any (often empty for this build)
   - `diagnostics`: lexical/syntax/semantic/runtime classification
6. Add human-friendly suggestions (`humanMessage`, `fixHint`).
7. Return:
   - `200` when no diagnostics/stderr
   - `422` when compile/runtime diagnostics exist
   - `500` for infrastructure failures
8. Cleanup temp file in `finally`.

Diagnostic classifier supports patterns:
- `LEXICAL ERROR ...`
- `SYNTAX ERROR ...`
- `Semantic Error ...`
- `RUNTIME ERROR ...`

## 3.4 Playground page (`web/src/app/playground/page.tsx`)

Main responsibilities:
- Holds editor code and compile results state.
- Runs health checks on mount and on demand.
- Calls `/api/compile` on Run or `Ctrl/Cmd + Enter`.
- Routes results to output tabs (`output`, `tokens`, `ir`, `errors`).
- Tracks cursor line/column and run duration.

## 3.5 Monaco editor panel (`CodeEditorPanel.tsx`)

- Registers custom AstroScript language.
- Defines syntax highlighting for mission keywords/operators/comments.
- Displays diagnostics as Monaco markers.
- Supports jump-to-line from error panel.

## 3.6 Output panel (`OutputPanel.tsx`)

- Tabbed display of runtime output, tokens, IR, errors.
- Error tab renders diagnostic cards with line jump support.
- Shows user-friendly summary and fix hints from compile API.

---

## 4) End-to-End Sequence Diagram (Textual)

1. User writes mission code in Monaco.
2. User clicks Run.
3. Frontend checks compiler health state.
4. Frontend sends code to `/api/compile`.
5. API writes temp file and executes binary.
6. Compiler: lexer -> parser+semantic actions -> TAC optimize -> TAC execute.
7. Compiler emits:
   - IR section
   - `PRINT:` lines
   - semantic/runtime errors (if any)
8. API parses and enriches diagnostics.
9. Frontend renders output/IR/errors and editor markers.

---

## 5) What Was Audited and Updated

During this pass, the backend was hardened for consistency and safer failure modes:

1. Added function call arity mismatch semantic check in parser.
2. Added runtime guards/errors in TAC interpreter for:
   - divide/modulo by zero
   - array bounds and undeclared array references
   - invalid labels/jumps
   - undefined function calls
   - stack overflow guard
   - memory allocation failures
3. Added executable support in parser/runtime for documented operators:
   - `mod`
   - `**`
   - `AND`, `OR`, `XOR`, `NOT`
4. Activated previously partial tokens and semantics:
   - `deploy`, `this`, `broadcast`, `alarm`
   - math built-ins: `root`, `flr`, `ceil`, `abs`, `logarithm`, `sine`, `cosine`, `tan`, `asine`, `acosine`, `atan`, `prime`
   - loop/switch controls: `stage_sep`, `coast`
   - declaration tokens: `alias`, `fleet`
5. Added stronger static checks:
   - assignment/initialization type compatibility
   - return type validation
   - constant reassignment prevention

This reduces drift between docs and implementation while improving robustness under edge conditions.

---

## 6) Remaining Gaps (Current Reality)

These are important for accurate explanation to an instructor:

1. Static type checks are now significantly stronger, but not yet equivalent to a full production-grade static type system.
2. Object-oriented runtime remains lightweight: module/deploy/this are active, but full class object memory/method dispatch semantics are intentionally simple.
3. Advanced exceptions such as Java-style null/class-cast are mostly not applicable to the current runtime model.

---

## 7) Practical Explanation Script (For Teacher Presentation)

Use this short flow while presenting:

1. "AstroScript is compiled by Flex+Bison into TAC, then interpreted."
2. "The parser is not only validating syntax, it also emits IR instructions directly."
3. "Semantic checks currently focus on symbol existence and call arity, while runtime checks catch unsafe execution cases."
4. "The web playground calls an API route that executes the compiler binary and converts raw output into structured diagnostics and tabs."
5. "So students can inspect both final output and intermediate representation in one place, which is ideal for learning compiler internals."
