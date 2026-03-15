export default function DocsArchitecture() {
  return (
    <article>
      <h1>Compiler Architecture</h1>

      <p>
        AstroScript&apos;s compiler follows a classic multi-phase compilation pipeline
        implemented using Flex (lexer), Bison (parser), and C++.
      </p>

      <h2>Pipeline Overview</h2>
      <pre><code>{`Source Code (.as)
    │
    ▼
┌──────────┐
│  Lexer   │  (Flex)
└────┬─────┘
     │ tokens
     ▼
┌──────────┐
│  Parser  │  (Bison)
└────┬─────┘
     │ semantic actions
     ▼
┌────────────────┐
│ Semantic Check │  (symbol_table)
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ IR Generation  │  (tac - Three Address Code)
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ Optimization   │
└────┬───────────┘
     │
     ▼
┌────────────────┐
│ Execution      │
│ TAC Interpreter│
└────────────────┘`}</code></pre>

      <h2>1. Lexical Analysis (Flex)</h2>
      <p>
        The lexer (<code>backend/compiler/lexer/lexer.l</code>) converts source code into tokens.
        It handles keywords, operators, literals (integers, floats, strings), identifiers,
        and comments.
      </p>

      <h2>2. Parsing (Bison)</h2>
      <p>
        The LALR(1) parser (<code>backend/compiler/parser/parser.y</code>) validates syntax and
        triggers semantic actions. It defines the grammar for all AstroScript constructs including
        declarations, assignments, control flow, loops, functions, and modules.
      </p>

      <h2>3. Semantic Analysis</h2>
      <p>
        The symbol table (<code>backend/compiler/semantic/</code>) tracks variable names, types,
        and detects errors like duplicate declarations and undeclared variable usage.
      </p>

      <h2>4. Intermediate Code Generation</h2>
      <p>
        The TAC generator (<code>backend/compiler/ir/</code>) produces three-address code
        instructions for arithmetic, control flow, function calls, array operations, and I/O.
      </p>

      <h2>5. Optimization</h2>
      <p>Three optimization passes are applied to the TAC:</p>
      <ul>
        <li><strong>Constant folding</strong> — evaluates constant expressions at compile time</li>
        <li><strong>Algebraic simplification</strong> — removes identity operations (x+0, x*1)</li>
        <li><strong>Redundant move elimination</strong> — removes self-assignments</li>
      </ul>

      <h2>6. Execution</h2>
      <p>
        The TAC interpreter executes the optimized instructions using a stack-based runtime
        with call frames for function scope, array storage, and a parameter passing stack.
      </p>

      <h2>Directory Structure</h2>
      <pre><code>{`backend/compiler/
├── lexer/          # Flex lexer definition
├── parser/         # Bison parser grammar
├── semantic/       # Symbol table
├── ir/             # Three-address code generator
├── include/        # Shared headers (future)
├── main.cpp        # Compiler entry point
└── build/          # Generated files and binary`}</code></pre>
    </article>
  );
}
