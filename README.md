# AstroScript

A space-themed programming language with a custom compiler built using Flex, Bison, and C++. Includes a web-based playground powered by Next.js.

## Project Structure

```
astroscript/
├── backend/
│   └── compiler/
│       ├── lexer/          # Flex lexer definition
│       ├── parser/         # Bison parser grammar
│       ├── semantic/       # Symbol table (semantic analysis)
│       ├── ir/             # Three-address code generator
│       ├── include/        # Shared headers
│       ├── main.cpp        # Compiler entry point
│       ├── build/          # Generated files & binary
│       └── Legacy/         # Previous compiler iterations
├── web/                    # Next.js website & playground
├── docs/                   # Language documentation
├── examples/               # Example AstroScript programs
└── scripts/                # Build & utility scripts
```

## Quick Start

### Build the Compiler

Requires: `flex`, `bison`, `g++` (with C++17 support)

```bash
./scripts/build_compiler.sh
```

### Run a Program

```bash
./backend/compiler/build/astroscript examples/comprehensive.as
```

### Run the Website

Requires: Node.js 18+

```bash
cd web
npm install
npm run dev
```

Then open [http://localhost:3000](http://localhost:3000) to access the homepage and playground.

## Hello World

```
mission HelloWorld launch {
    transmit "Hello, Universe!".
} success
```

## Language Highlights

- **Space-themed keywords**: `mission`, `telemetry`, `transmit`, `verify`, `orbit`, `command`
- **Types**: `count` (int), `real` (float), `precise` (double), `flag` (bool), `symbol` (string)
- **Control flow**: `verify`/`else_verify`/`otherwise`, `scenario`/`trajectory`/`fallback`
- **Loops**: `orbit` (while), `orbit times` (for)
- **Functions**: `command` definitions with `back` returns
- **Compiler phases**: Lexing → Parsing → Semantic Analysis → TAC Generation → Optimization → Execution

## Documentation

See the [docs/](docs/) directory or run the website and navigate to `/docs`.

## License

See [LICENSE](LICENSE).
