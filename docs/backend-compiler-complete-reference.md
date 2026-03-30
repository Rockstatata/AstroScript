# AstroScript Backend Compiler: Complete Documentation

This document is a full backend-focused reference for the AstroScript compiler implementation in `backend/compiler`, with packaging/build/runtime integration notes needed to use it as an actual compiler in your project.

It is intentionally structured in four layers, exactly as requested:

1. Very simple explanation (easy words)
2. Higher-level technical overview
3. Full process flow + purpose of each file/process
4. Deep, implementation-level documentation with line landmarks and subsystem behavior

## March 30, 2026 Verification Notes

This reference is still valid conceptually, with the following concrete updates verified against the active source:

1. Main pipeline now includes C-like translation printing between optimized TAC and runtime execution:
  - `tacGenerator.printCode(...)`
  - `tacGenerator.printCTranslation(...)`
  - `tacGenerator.execute()`
2. C translation has been improved to preserve side-effecting call/method instructions and render visible argument lists at translated call sites.
3. Scope-aware semantic checks are primarily implemented in parser helper/state logic; `symbol_table.cpp` remains a lightweight metadata store and reporter.
4. For exact feature-to-line execution mapping (conditionals, loops, functions, classes/modules), use:
  - `docs/compiler-feature-execution-map.md`

---

## 1) Easy Explanation (Simple Words)

Think of AstroScript backend as a factory with six rooms:

1. **Read room**: It reads your `.as` code file.
2. **Word room (Lexer)**: It cuts your code into words/symbols (tokens), like `mission`, `telemetry`, `+`, `(`, `)`.
3. **Grammar room (Parser)**: It checks whether those words are in a valid AstroScript sentence order.
4. **Meaning room (Semantic checks)**: It checks logic mistakes like using a variable before declaring it.
5. **Instruction room (TAC/IR)**: It converts code into very simple machine-like steps (three-address code).
6. **Run room (Interpreter)**: It runs those simple steps and prints output/errors.

So, your AstroScript code is not sent directly to CPU machine code. Instead, it is translated into an intermediate instruction list (TAC), optimized, and then interpreted.

### How keywords get "life"

Every keyword gets life in 3 places:

1. **Lexer rule** gives it a token name.
2. **Parser grammar rule** says where it is legal in syntax.
3. **Semantic/TAC action** defines what it actually does at runtime.

Example:

- `transmit` is recognized by lexer.
- Parser allows `transmit expression .` as a statement.
- TAC emits a `print` instruction.
- Runtime executes `print` and you see `PRINT: ...`.

### How it becomes a usable compiler

1. Build scripts run Flex + Bison + g++.
2. Output binary is created (`astroscript` / `astroscript.exe`).
3. Web API calls that binary with a temporary source file.
4. API parses output and sends clean diagnostics/results to frontend.

That is how the backend compiler powers your playground.

---

## 2) Higher-Level Technical Overview

### Compilation model

AstroScript backend is a hybrid **compile + interpret** architecture:

- Frontend language processing:
  - Lexing via Flex (`lexer.l`)
  - Parsing via Bison (`parser.y`)
  - Inline semantic validation
  - Immediate TAC emission
- Mid-end:
  - TAC optimization passes
- Back-end runtime:
  - TAC interpreter with runtime frames and arrays

### Main characteristics

- **Grammar-driven code generation**: parser semantic actions emit TAC directly.
- **Single-pass semantics (mostly)**: many semantic checks happen while parsing.
- **Instruction-level runtime**: no native machine code generation.
- **Runtime safety checks**: divide/modulo by zero, bad array access, invalid jump targets, stack overflow guards.

### Active backend modules

- `backend/compiler/lexer`: lexical rules.
- `backend/compiler/parser`: grammar + semantic actions.
- `backend/compiler/semantic`: symbol table.
- `backend/compiler/ir`: TAC model, optimization, execution engine.
- `backend/compiler/main.cpp`: entry point and orchestration.
- `backend/compiler/build`: generated parser/lexer C files and compiled binaries.

### Historical module

- `backend/compiler/Legacy/progress-*`: milestone snapshots of previous compiler iterations.

---

## 3) Total Flow + Purpose of Each File and Process

## 3.1 End-to-end flow (source to output)

1. User runs compiler with source path.
2. `main.cpp` sets `yyin` input and calls `yyparse()`.
3. Lexer (`lexer.l`) tokenizes source code.
4. Parser (`parser.y`) matches grammar and emits TAC.
5. Semantic checks print errors if needed.
6. If parse status is success (`0`):
   - optimize TAC
   - print optimized TAC
  - print C-like translation
   - execute TAC
   - print symbol table
7. Process returns parse status.

## 3.2 Runtime output channels

- Standard output:
  - TAC dump section (`--- Optimized Three Address Code ---`)
  - runtime prints (`PRINT: ...`)
  - symbol table dump
- Error output and/or printed error lines:
  - lexical/syntax/semantic/runtime diagnostics

## 3.3 Purpose of active files

### Core entry

- `backend/compiler/main.cpp`
  - Compiler driver.
  - Connects parser result to optimization/execution/symbol dump.

### Lexing

- `backend/compiler/lexer/lexer.l`
  - Token definitions for all language keywords/operators/literals.
  - Comment skipping.
  - lexical error reporting.
  - integer literal parsing helper.

### Parsing and semantic actions

- `backend/compiler/parser/parser.y`
  - Full grammar for mission blocks, declarations, assignments, control flow, loops, switch-like scenarios, modules, functions, expressions.
  - Semantic state containers (if/loop/switch stacks, function metadata, aliases, constants).
  - Type compatibility checks.
  - Function arity and return checks.
  - TAC emission at parse time.

### Semantic storage

- `backend/compiler/semantic/symbol_table.h`
  - Symbol struct and SymbolTable API.
- `backend/compiler/semantic/symbol_table.cpp`
  - Symbol insertion, lookup, type retrieval, value storage, table printing.

### TAC IR and interpreter

- `backend/compiler/ir/tac.h`
  - TAC instruction structure and TACGenerator interface.
- `backend/compiler/ir/tac.cpp`
  - TAC emitters.
  - constant/algebraic optimization passes.
  - pretty-printer for TAC.
  - runtime interpreter with function call stack and array memory.
  - runtime domain/safety error checks.

### Generated/build artifacts

- `backend/compiler/build/lex.yy.c`
  - Generated lexer C source from Flex.
- `backend/compiler/build/parser.tab.c`
  - Generated parser C source from Bison.
- `backend/compiler/build/parser.tab.h`
  - Generated token header from Bison.
- `backend/compiler/build/astroscript`
  - Linux binary.
- `backend/compiler/build/astroscript.exe`
  - Windows binary.

### Build and packaging scripts (project-level, backend-serving)

- `scripts/build_compiler.sh`
  - Linux build pipeline (Flex/Bison/g++).
  - GLIBC compatibility guard for deployment safety.
  - syncs Linux binary to web server compiler folder.
- `scripts/build_compiler.ps1`
  - Windows build pipeline.
  - syncs Windows binary to web server compiler folder.

### Integration layer for using backend as app compiler (web)

- `web/src/lib/compilerPath.ts`
  - runtime compiler-binary candidate resolution and caching.
  - current code has `ASTROSCRIPT_COMPILER_TARGET` hardcoded to `linux`.
- `web/src/app/api/compile/route.ts`
  - writes temp `.as`, runs compiler binary, parses output/errors.
- `web/src/app/api/health/route.ts`
  - checks binary availability and metadata.

## 3.4 Purpose of legacy files

Each `backend/compiler/Legacy/progress-*` folder is a timeline snapshot:

- `progress-1`: early lexer/token-output stage.
- `progress-2`: parser stage with parser outputs and samples.
- `progress-3`: introduces symbol table semantics.
- `progress-4`: richer grammar and samples.
- `progress-5`: includes TAC model and fuller compiler behavior.

They are historical references, not the active production backend path.

---

## 4) Deep and Comprehensive Technical Documentation

This section goes subsystem by subsystem with implementation detail and landmarks.

## 4.1 `main.cpp` (Driver and orchestration)

### Responsibilities

- Expose external parser globals and shared module singletons.
- Open input file and route to lexer (`yyin`).
- Trigger parse.
- On successful parse, trigger post-parse pipeline.

### Landmarks

- `int main(...)`: line 13
- `yyparse()`: line 26
- `tacGenerator.optimize()`: line 28
- `tacGenerator.execute()`: line 30
- `symbolTable.printTable()`: line 31

### Behavior details

- If file cannot be opened, immediate error + exit code 1.
- If syntax parse fails, optimize/execute stages are skipped.
- Exit code equals parse status.

---

## 4.2 Lexer internals (`lexer.l`)

## 4.2.1 Tokenization model

- Uses `%option yylineno` for line-aware diagnostics.
- Returns token enums defined/consumed by parser.
- Captures semantic values via `yylval` union.

## 4.2.2 Integer literal handling

- `parse_int_literal(const char* text)` at line 17.
- Manual parse + clamping to `INT_MIN..INT_MAX`.
- Avoids unsafe conversions and helps portability.

## 4.2.3 Keyword/token coverage

- Mission/program keywords.
- Module/OOP-style keywords.
- Declaration/type keywords.
- I/O and control flow keywords.
- Loop/switch-like keywords.
- Math function keywords.
- Operators, punctuation, assignment and return-type tokens.

## 4.2.4 Literals and identifiers

- Integer: `INT_LITERAL` at line 52 pattern.
- Float: `FLOAT_LITERAL` pattern.
- String: quoted string pattern.
- Identifier: letters/underscore + alnum/underscore.

## 4.2.5 Comment styles

- Single-line style beginning with `$$`.
- Block style `$* ... *$`.

## 4.2.6 Lexical error behavior

- Unknown character path prints:
  - `LEXICAL ERROR at line %d: %s`
- Landmark: line 183.

---

## 4.3 Parser internals (`parser.y`)

`parser.y` is the largest and most behavior-rich file in backend.

## 4.3.1 Parse-time global state

The parser maintains multiple runtime semantic stacks/maps during parse:

- `ifStack`: if/else chain control labels.
- `loopStack`: loop boundaries and continue/end labels.
- `switchStack`: scenario/trajectory/fallback labels.
- function metadata maps:
  - arity
  - parameter types
  - return types
- type aliases map.
- constants set.
- module depth and function context flags.

This is how parser semantic actions carry context across grammar productions.

## 4.3.2 Token and precedence declarations

- `%token` block starts around line 170.
- Operator precedence and associativity declarations provide expression parsing order.

## 4.3.3 Grammar anchors

- `program`: line 216
- `statement`: line 233
- `declaration`: line 271
- `assignment`: line 346
- `control_statement`: line 492
- `loop_statement`: line 538
- `switch_statement`: line 605
- `function_definition`: line 691
- `expression`: line 779
- `yyerror`: line 1115

## 4.3.4 Declaration semantics

Supported forms:

- `telemetry type name .`
- `telemetry type name := expression .`
- `limit type name := expression .`
- typed arrays with fixed length and optional first assignment.
- type alias declarations.
- mode declarations.

Checks performed:

- duplicate declaration guard
- type compatibility between declared type and initializer
- constant registration (`limit`)

TAC emitted:

- `decl`
- `decl_arr`
- assignment emit when initializer exists

## 4.3.5 Assignment semantics

Scalar assignment:

- checks declared name
- blocks assignment to constants
- checks type compatibility
- emits `=` TAC

Array assignment:

- checks array declaration
- checks element type compatibility
- emits `store`

## 4.3.6 Input/Output semantics

- `transmit`, `broadcast`, `alarm` all map to print emit.
- `receive` maps to input emit for scalar and temporary+store for array index.

## 4.3.7 Control-flow semantics

### `verify / else_verify / otherwise`

- create labels for else and end.
- emit `ifFalse` jump.
- emit branch body.
- chain `else_verify` with new labels.
- finalize end label.

### `orbit (...)` and `orbit while (...)`

- start/end labels created.
- emit condition check via `ifFalse`.
- body then jump back to start.

### `orbit times(...)`

- emits iterator init.
- emits `<` condition.
- emits increment in continue block.

### `stage_sep` and `coast`

- `stage_sep`: jump to loop or switch end label.
- `coast`: jump to loop continue label.
- semantic errors if used outside proper context.

## 4.3.8 Switch-like semantics (`scenario`)

- stores expression place in `SwitchContext`.
- each `trajectory` emits compare + conditional jump to next label.
- each case body ends with jump to switch end.
- `fallback` marks default body.

## 4.3.9 Function/module semantics

Function pipeline:

1. Parse signature.
2. Collect parameter names/types.
3. Register arity, parameter type map, return type.
4. Emit `func_begin` + `param_def` entries.
5. Parse body.
6. Enforce return expectations.
7. Emit `return` + `func_end`.

Return rules:

- `back .` is legal only for `VOIDSPACE` returns.
- `back expression .` checked against declared return type.
- non-void function without value return emits semantic warning.

## 4.3.10 Expression semantics

Expression branch handles:

- comparisons (`< > <= >= == !=`) -> result type `FLAG`
- arithmetic (`+ - * / % **`) -> numeric result type infer rules
- logical (`AND OR XOR NOT`) -> `FLAG`
- built-in math unary calls (`root`, `flr`, etc.)
- literals (int/float/string)
- identifiers and array loads
- function calls with arity validation
- parenthesized expressions

Type checks are present for most operator classes and function calls.

---

## 4.4 Symbol table internals (`symbol_table.*`)

## 4.4.1 Data model

- `Symbol`:
  - `name`
  - `type`
  - `value` (double)

- internal table:
  - `unordered_map<string, Symbol>`

## 4.4.2 API behavior

- `insert` (line 5 in cpp)
  - rejects duplicates via `exists`.
- `exists` (line 14)
  - O(1) lookup.
- `getValue` / `setValue`
  - runtime numeric value path.
- `getType` (line 33)
  - type lookup.
- `printTable` (line 41)
  - prints all symbols.

Note: runtime execution primarily uses TAC frames/arrays; this table is mostly semantic/declaration metadata plus final reporting.

---

## 4.5 TAC IR and execution internals (`tac.h` + `tac.cpp`)

## 4.5.1 TAC instruction model

Each instruction has 4 strings:

- `op`
- `arg1`
- `arg2`
- `result`

Examples:

- `decl COUNT x`
- `t3 = t1 + t2`
- `ifFalse t4 goto L2`
- `x[0] = t5`

## 4.5.2 Emission API landmarks

- `emit`: line 70
- `emitBinary`: line 74
- `emitUnary`: line 80
- `emitFunctionBegin`: line 128
- `emitCall`: line 144

These are used directly by parser semantic actions.

## 4.5.3 Runtime value model

- `RuntimeValue = variant<double, string>`
- functions like `valueToNumber`, `valueToString`, `valueToBool` convert appropriately.
- this allows numeric and string behavior in same interpreter.

## 4.5.4 Binary operation engine

`evalBinary`:

- arithmetic with string concat support for `+`
- modulo via `portableModulo`
- logic operators and relational operators
- equality/inequality handles both string and numeric modes
- divide/modulo by zero guarded with runtime error messages

Landmarks:

- divide-by-zero message: line 282
- modulo-by-zero message: line 289

## 4.5.5 Optimization passes

### constantFold (line 340)

- fold numeric binary operations when operands are immediate numerics.
- fold unary math operations when arg is immediate numeric and domain valid.
- replaces instruction with direct assignment when fold succeeds.

### algebraicSimplify (line 459)

- identity simplifications and zero/one simplifications.

### removeRedundantMoves (line 497)

- removes self-assignments.

### optimize (line 511)

- executes all three passes in sequence.

## 4.5.6 Pretty printing

`printCode` (line 517):

- prints a normalized textual view of each TAC op.
- used by frontend API to extract IR section.

## 4.5.7 Execution engine

`execute` starts at line 570.

### Metadata stage

- build label index and function bounds before run.

### Runtime stores

- frame stack for variables.
- arrays map.
- call stack with return program counter and return temp.
- parameter stack.

### Core op dispatch categories

- flow: `label`, `goto`, `ifFalse`
- declaration/assign: `decl`, `decl_arr`, `=`
- I/O: `print`, `input`
- arrays: `store`, `load`
- function call path: `param`, `func_begin`, `call`, `return`, `func_end`, `param_def`
- delay op: `wait`
- math built-ins unary ops
- binary operators

### Runtime safeguards (landmarks)

- runtimeError lambda: line 587
- invalid jump target: line 602
- invalid conditional jump target: line 614
- negative/oversized array size: lines 633, 637
- bad array reference: lines 680, 698
- out-of-bounds index: lines 685, 704
- undefined function: line 735
- stack overflow: line 742
- invalid arg count parse: line 750
- invalid frame state: lines 787, 805
- domain errors for root/log/asine/acosine: lines 836, 850, 864, 872

This is a robust interpreter with explicit error surfaces and continuation in many non-fatal cases.

---

## 4.6 Build and packaging details

## 4.6.1 Linux script (`scripts/build_compiler.sh`)

Pipeline:

1. Resolve root/build/server-compiler directories.
2. Flex generates `lex.yy.c`.
3. Bison generates `parser.tab.c` + `parser.tab.h`.
4. g++ compiles:
   - generated lexer/parser C files
   - semantic + TAC + main C++ files
5. Guard checks dynamic symbols:
   - if GLIBC 2.38+ symbols detected, exits with error
6. Copy built binary to web server compiler path as `astroscript-linux`.
7. `chmod +x` the target.

This guard is specifically to prevent deployment breakage on runtimes with older libc than the build machine.

## 4.6.2 Windows script (`scripts/build_compiler.ps1`)

Pipeline:

1. Resolve root/build/server-compiler directories.
2. Flex/Bison generate parser/lexer outputs.
3. g++ compiles to `astroscript.exe`.
4. Copy to `web/src/server/compiler/astroscript-windows.exe`.

## 4.6.3 Binary files and their role

- `backend/compiler/build/astroscript`:
  - canonical Linux build output.
- `backend/compiler/build/astroscript.exe`:
  - canonical Windows build output.
- synced copies in web server folder:
  - consumed by app API runtime path resolution.

---

## 4.7 Keyword lifecycle: from text to runtime effect

A keyword becomes real behavior through this chain:

1. Lexer rule matches text and returns token.
2. Parser `%token` declaration knows token type.
3. Grammar production uses token in legal syntax.
4. Semantic action emits TAC and/or performs checks.
5. Interpreter executes resulting TAC op.

### Example A: `transmit`

- Lexer maps `transmit` -> `TRANSMIT` token.
- Parser `print_stmt` allows `TRANSMIT expression DOT`.
- Action emits `emitPrint(expr.place)`.
- Runtime `print` op outputs `PRINT: value`.

### Example B: `verify`

- Lexer maps to `VERIFY`.
- Parser `control_statement` builds branch labels.
- Action emits `ifFalse` and branch jumps.
- Runtime evaluates condition and jumps accordingly.

### Example C: `orbit times`

- Lexer maps to `ORBIT` and `TIMES`.
- Parser constructs loop context with iterator + labels.
- Action emits init/condition/body/increment flow TAC.
- Runtime executes loop through label/goto dispatch.

### Example D: `root(...)`

- Lexer maps to `ROOT`.
- Parser expression branch emits unary TAC op `root`.
- Runtime executes with domain guard (`value >= 0`).

---

## 4.8 How the backend is used as a compiler in this project

## 4.8.1 CLI usage

- Build compiler binary.
- Run binary with `.as` file path.
- Parser and runtime execute full pipeline and print outputs.

## 4.8.2 Web API usage

- `compile` API receives source code text.
- writes temporary file.
- executes compiler binary via child process.
- parses output into structured response:
  - output
  - IR
  - C-like translation
  - diagnostics
- frontend visualizes everything in editor + tabs.

## 4.8.3 Compiler path resolution/caching

`web/src/lib/compilerPath.ts` controls discovery order and cache TTL.

Important current-state note:

- This file currently contains a hardcoded constant:
  - `const ASTROSCRIPT_COMPILER_TARGET = "windows"`
- That means runtime target preference is forced to Windows unless this constant is changed.
- If you want environment-driven switching, replace hardcoding with env-driven input.

---

## 4.9 Full backend file inventory and purpose matrix

## Active backend tree

- `backend/compiler/main.cpp` -> active entry.
- `backend/compiler/lexer/lexer.l` -> active lexer source.
- `backend/compiler/parser/parser.y` -> active parser/semantic source.
- `backend/compiler/semantic/symbol_table.h` -> active semantic interface.
- `backend/compiler/semantic/symbol_table.cpp` -> active semantic implementation.
- `backend/compiler/ir/tac.h` -> active IR/runtime interface.
- `backend/compiler/ir/tac.cpp` -> active IR/runtime implementation.
- `backend/compiler/build/lex.yy.c` -> generated from lexer.
- `backend/compiler/build/parser.tab.c` -> generated from parser.
- `backend/compiler/build/parser.tab.h` -> generated parser header.
- `backend/compiler/build/astroscript` -> Linux compiler binary.
- `backend/compiler/build/astroscript.exe` -> Windows compiler binary.
- `backend/compiler/include/` -> currently empty placeholder.

## Legacy tree

### `backend/compiler/Legacy/progress-1`

- `lexer.l`, `lex.yy.c`: initial lexical stage.
- sample inputs and token output reference files.

### `backend/compiler/Legacy/progress-2`

- parser added (`parser.y`, `parser.tab.*`, `parser.output`), lexer continued.
- sample test programs for grammar growth.

### `backend/compiler/Legacy/progress-3`

- symbol table introduced (`symbol_table.*`) with parser/lexer updates.

### `backend/compiler/Legacy/progress-4`

- richer grammar and semantics snapshot.

### `backend/compiler/Legacy/progress-5`

- TAC system (`tac.*`) included, near-modern architecture snapshot.

These legacy files are documentation-by-history and are useful for evolution analysis, regression study, and teaching milestones.

---

## 4.10 Common failure points and what they mean

1. `LEXICAL ERROR ...`
   - invalid character/token fragment from lexer.
2. `SYNTAX ERROR ...`
   - grammar mismatch from parser.
3. `Semantic Error ...`
   - declaration/type/usage mismatch from parser semantic actions.
4. `RUNTIME ERROR ...`
   - valid parse, but bad runtime condition in interpreter.
5. Linux deployment binary fails to start
   - usually libc compatibility mismatch; build script guard is intended to catch this.

---

## 4.11 Practical walkthrough (one statement lifecycle)

Take source statement:

`telemetry count x := 5 add 3.`

1. Lexer tokens:
   - TELEMETRY COUNT IDENTIFIER ASSIGN INT_LITERAL ADD INT_LITERAL DOT
2. Parser matches declaration-with-initialization rule.
3. Semantic checks:
   - duplicate declaration?
   - initializer type compatibility with `COUNT`?
4. TAC emitted:
   - `decl COUNT x`
   - `t1 = 5 + 3`
   - `x = t1`
5. Optimizer may fold `t1 = 8`.
6. Runtime assigns value into frame storage.
7. Symbol table printed at end includes `x`.

---

## 4.12 Minimal operational checklist (build -> run -> integrate)

1. Ensure Flex, Bison, g++ are installed.
2. Run build script (`.sh` or `.ps1`).
3. Confirm binary in `backend/compiler/build`.
4. Confirm synced binary in `web/src/server/compiler`.
5. Run compiler CLI on sample `.as` file.
6. Run web app and check `/api/health`.
7. Compile from playground and inspect output/errors/IR.

---

## 4.13 Closing summary

Your backend is a full educational compiler runtime stack:

- lexical analysis
- syntax parsing
- semantic validation
- TAC generation
- optimization
- interpretation

It is not a toy tokenizer; it has meaningful control-flow lowering, function call machinery, expression typing logic, array runtime semantics, and deployment-aware packaging scripts.

If you keep lexer tokens, parser productions, semantic checks, TAC emits, and interpreter op support synchronized, each keyword and language feature remains alive end-to-end.

---

## 4.14 IR and optimization explained for evaluation

This section is intended as a direct, instructor-facing explanation.

### 4.14.1 What IR is doing in AstroScript

After parsing and semantic checks, the compiler does not execute source syntax directly.
It first converts code into TAC (three-address code), where each instruction has:

- operation (`op`)
- first input (`arg1`)
- second input (`arg2`)
- output/target (`result`)

This turns high-level constructs into explicit low-level steps.

Examples:

- conditionals and loops become labels + conditional/unconditional jumps.
- function calls become `param` pushes + `call` + `return` handling.
- arrays become `decl_arr`, `store`, `load` operations.
- module/object behavior becomes `obj_new`, `field_get`, `field_set`, and `mcall`.

The TAC interpreter then executes this linear instruction stream with a program counter, frame stack, array store, object store, and call stack.

### 4.14.2 What optimization is doing in AstroScript

Before interpretation, TAC is optimized by three passes:

1. `constantFold`
  - evaluates compile-time numeric expressions and safe unary math built-ins.
2. `algebraicSimplify`
  - applies arithmetic identities (like `x+0`, `x*1`, `x/1`) and zero-product rules.
3. `removeRedundantMoves`
  - deletes no-op assignments (`a=a`).

Important: optimization is conservative for safety.
If folding would violate a domain rule (for example negative square root), the instruction is left for runtime checks.

### 4.14.3 Why this layer exists (the "why" answer)

1. Better compiler design
  - front-end concerns (grammar/typing) are separated from execution concerns.
2. Easier verification
  - TAC provides a clear, inspectable representation for debugging and grading.
3. Runtime efficiency
  - simple compile-time reductions reduce interpreter work.
4. Feature scalability
  - adding language features becomes "lower to TAC + execute TAC op".
5. Teaching value
  - students can observe source -> optimized IR -> execution, which exposes compiler internals clearly.

### 4.14.4 Short oral answer (ready to say)

"In our compiler, IR is the standardized executable form of the language. The parser lowers AstroScript statements into TAC instructions, and then a small optimizer removes constant and redundant work before execution. We do this to separate syntax from runtime, improve clarity, reduce runtime cost, and make the whole compiler pipeline observable for debugging and learning."
