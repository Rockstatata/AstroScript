# Introduction to AstroScript

AstroScript is a space-themed programming language designed for educational purposes. It demonstrates core compiler construction concepts including lexical analysis, parsing, semantic analysis, intermediate code generation, and optimization.

## Philosophy

AstroScript replaces traditional programming keywords with space mission terminology:

- **mission** — program entry point
- **telemetry** — variable declarations
- **transmit** — print output
- **verify** — conditional statements
- **orbit** — loops
- **command** — function definitions

## Hello World

```
mission HelloWorld launch {
    transmit "Hello, Universe!".
} success
```

Every AstroScript program is wrapped in a `mission ... launch { } success` block, representing a complete space mission from launch to successful completion.

## Features

- Static type system with types like `count`, `real`, `flag`, and `symbol`
- Control flow with `verify`/`else_verify`/`otherwise`
- Loops: `orbit` (while), `orbit while`, and `orbit times` (for)
- Functions via `command` definitions
- Arrays and string support
- Three-address code generation with optimization
- Built-in runtime interpreter
