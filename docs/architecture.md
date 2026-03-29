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
