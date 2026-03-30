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

Structured build with Make (recommended):

```bash
make linux
make windows
# or build both
make all
```

macOS/Linux:

```bash
./scripts/build_compiler.sh
```

Windows (PowerShell):

```powershell
.\scripts\build_compiler.ps1
```

Equivalent mapping between script and Make targets:

- `./scripts/build_compiler.sh` ↔ `make linux`
- `.\scripts\build_compiler.ps1` ↔ `make windows`

Build script outputs:

- Windows binary is copied to `web/src/server/compiler/astroscript-windows.exe`
- Linux binary is copied to `web/src/server/compiler/astroscript-linux`

### Run a Program

```bash
./backend/compiler/build/astroscript examples/comprehensive.as
```

Windows:

```powershell
.\backend\compiler\build\astroscript.exe .\examples\comprehensive.as
```

Run focused module examples (nested loops, nested conditionals, nested function calls, and more):

```bash
./backend/compiler/build/astroscript examples/modules/14-nested-loops-grid.as
./backend/compiler/build/astroscript examples/modules/15-nested-if-else.as
./backend/compiler/build/astroscript examples/modules/16-nested-function-calls.as
```

The frontend playground example list is generated from `examples/modules/index.json` and `examples/modules/*.as`.
Any module example you add there is picked up automatically by `npm run dev` and `npm run build` in `web/`.

### Run the Website

Requires: Node.js 20.11+

```bash
cd web
npm install
npm run dev
```

The web app now regenerates docs keyword/syntax reference data from compiler sources on `dev` and `build` automatically.

Compiler health can be checked via:

```bash
GET /api/health
```

Then open [http://localhost:3000](http://localhost:3000) to access the homepage and playground.

### Toggle Compiler Target

Use `ASTROSCRIPT_COMPILER_TARGET` to switch binaries without code changes.

PowerShell:

```powershell
$env:ASTROSCRIPT_COMPILER_TARGET = "windows"
# or
$env:ASTROSCRIPT_COMPILER_TARGET = "linux"
```

bash:

```bash
export ASTROSCRIPT_COMPILER_TARGET=windows
# or
export ASTROSCRIPT_COMPILER_TARGET=linux
```

If unset, the resolver defaults to `auto` and picks based on OS.

### Vercel Deployment Notes

- The web app is deployment-ready with Next.js standalone output and route runtime settings.
- API routes `/api/compile` and `/api/health` require the AstroScript compiler binary to exist at runtime.
- Provide `ASTROSCRIPT_COMPILER_PATH` if your binary is stored outside the default paths.
- If the binary is unavailable in the deployment environment, docs/UI still work and health endpoint reports compiler offline.

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
- **Learning bridge**: Optimized TAC plus readable C-like translation output for playground comparison

## Documentation

See the [docs/](docs/) directory or run the website and navigate to `/docs`.

Rubric tracking is maintained in [docs/rubric-compliance.md](docs/rubric-compliance.md).

Line-by-line compiler tracing for conditionals, loops, functions, and classes/modules is available in [docs/compiler-feature-execution-map.md](docs/compiler-feature-execution-map.md).

## License

See [LICENSE](LICENSE).
