# Compiler Architecture

AstroScript's compiler follows a classic multi-phase compilation pipeline.

## Pipeline Overview

```
Source Code (.as)
    │
    ▼
┌──────────┐
│  Lexer   │  (Flex)
│  lexer.l │
└────┬─────┘
     │ tokens
     ▼
┌──────────┐
│  Parser  │  (Bison)
│ parser.y │
└────┬─────┘
     │ semantic actions + TAC emission
     ▼
┌────────────────┐
│ Semantic Check │  (parser.y + symbol_table.cpp)
│ Symbol Metadata│
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ IR Generation  │  (tac.cpp)
│ Three-Address  │
│ Code (TAC)     │
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ Optimization   │
│ - Const fold   │
│ - Algebraic    │
│ - Redundant mv │
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ C-Like Output  │
│ Readable code  │
│ for playground │
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ Execution      │
│ TAC Interpreter│
└────────────────┘
```

## Phase Details

### 1. Lexical Analysis (Flex)

File: `backend/compiler/lexer/lexer.l`

Converts source code into tokens. Handles:
- Keywords (mission, telemetry, verify, orbit, etc.)
- Operators (add, minus, mul, :=, etc.)
- Literals (integers, floats, strings)
- Identifiers
- Comments ($$ and $* ... *$)

### 2. Parsing (Bison)

File: `backend/compiler/parser/parser.y`

LALR(1) parser that validates syntax and triggers semantic actions. Defines the grammar for all AstroScript constructs including declarations, assignments, control flow, loops, functions, and modules.

### 3. Semantic Analysis

Files: `backend/compiler/semantic/symbol_table.h`, `symbol_table.cpp`

Maintains symbol metadata storage (name, type, declared line) and final symbol reporting.

Important: scope-aware declaration checks and most semantic validation logic are implemented in `parser.y` (for example `declareScopedName`, `isDeclaredName`, overload resolution, module inheritance checks, and member access checks).

### 4. Intermediate Code Generation

Files: `backend/compiler/ir/tac.h`, `tac.cpp`

Generates three-address code (TAC) instructions:
- Arithmetic operations
- Control flow (labels, gotos, conditional jumps)
- Function calls and returns
- Array operations
- I/O operations

### 5. Optimization

Applied to TAC before execution:
- **Constant folding** — evaluates constant expressions at compile time
- **Algebraic simplification** — removes identity operations (x+0, x*1, etc.)
- **Redundant move elimination** — removes self-assignments

### 6. C-Like Translation (Learning View)

File: `backend/compiler/ir/tac.cpp`

The backend now prints a readable C-like projection generated from optimized TAC.
This output is designed for learning and playground comparison, so users can map AstroScript constructs to familiar C-style statements.

Current behavior (March 30, 2026):
- Function and method call arguments are now carried through into translated call sites.
- Side-effecting calls are preserved in output even when temporary results are not reused.
- Constructor/method invocation emitted after object creation now keeps the original argument list in translation.
- The output remains a learning-focused pseudo-C projection, not a strict production transpiler.

### 7. Execution

The TAC interpreter executes the optimized instructions using a stack-based runtime with:
- Call frames for function scope
- Array storage
- Parameter passing stack

## Directory Structure

```
backend/compiler/
├── lexer/          # Flex lexer definition
├── parser/         # Bison parser grammar
├── semantic/       # Symbol table
├── ir/             # Three-address code generator
├── include/        # Shared headers (future)
├── main.cpp        # Compiler entry point
└── build/          # Generated files and binary
```

## Execution Map

For exact lexer/parser/symbol/TAC line-by-line execution points for conditionals, loops, functions, and classes, see:

- `docs/compiler-feature-execution-map.md`

## Why IR and Optimization Exist (Teacher-Ready)

### What IR is in this compiler

AstroScript uses a three-address intermediate representation (TAC) as a bridge between parsing and runtime.

- Each IR instruction is a compact 4-field record: operation + up to two inputs + one output.
- Examples of operations include assignment, arithmetic, control flow jumps, function calls, array access, object field access, and built-in math functions.
- Parser semantic actions emit these TAC instructions directly while reducing grammar rules.

So instead of executing syntax-tree nodes directly, the compiler first rewrites source constructs into a uniform low-level instruction list.

### What optimization is doing

Before execution, `optimize()` applies three passes in order:

1. `constantFold()`
     - Computes expressions early when both operands are compile-time numeric constants.
     - Also folds unary math built-ins when domain-safe (for example avoids folding invalid `root(-x)` or `logarithm(0)`).
2. `algebraicSimplify()`
     - Applies identity rules like `x + 0 -> x`, `x * 1 -> x`, `x * 0 -> 0`, `x / 1 -> x`.
3. `removeRedundantMoves()`
     - Removes no-op moves such as `x = x`.

These passes are local and semantics-preserving for the supported cases.

### Why we do this (core justification)

1. Separation of concerns
     - Front-end (syntax and semantic checks) is separated from runtime execution.
2. Target independence
     - The same IR can be interpreted now and can later support additional backends if needed.
3. Easier correctness and debugging
     - TAC is explicit (`label`, `goto`, `ifFalse`, `call`), so control/data flow is visible and testable.
4. Better performance and cleaner output
     - Constant/algebraic simplifications reduce runtime work.
5. Better pedagogy
     - Students can inspect optimized TAC and C-like translation, then compare with source behavior.

In short: IR is the canonical executable form, and optimization is an early clean-up step that reduces unnecessary runtime effort without changing intended program behavior.
