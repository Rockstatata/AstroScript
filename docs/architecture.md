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
     │ AST / semantic actions
     ▼
┌────────────────┐
│ Semantic Check │  (symbol_table.cpp)
│ Symbol Table   │
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

Maintains a symbol table that tracks:
- Variable names and types
- Scope information
- Duplicate declaration detection
- Undeclared variable usage detection

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

### 6. Execution

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
