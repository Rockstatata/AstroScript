export default function DocsIntroduction() {
  return (
    <article>
      <h1>Introduction to AstroScript</h1>

      <p>
        AstroScript is a space-themed programming language designed for educational purposes.
        It demonstrates core compiler construction concepts including lexical analysis, parsing,
        semantic analysis, intermediate code generation, and optimization.
      </p>

      <h2>Philosophy</h2>
      <p>
        AstroScript replaces traditional programming keywords with space mission terminology:
      </p>
      <ul>
        <li><strong>mission</strong> — program entry point</li>
        <li><strong>telemetry</strong> — variable declarations</li>
        <li><strong>transmit</strong> — print output</li>
        <li><strong>verify</strong> — conditional statements</li>
        <li><strong>orbit</strong> — loops</li>
        <li><strong>command</strong> — function definitions</li>
      </ul>

      <h2>Hello World</h2>
      <pre><code>{`mission HelloWorld launch {
    transmit "Hello, Universe!".
} success`}</code></pre>

      <p>
        Every AstroScript program is wrapped in a <code>mission ... launch {"{ }"} success</code> block,
        representing a complete space mission from launch to successful completion.
      </p>

      <h2>Features</h2>
      <ul>
        <li>Static type system with types like <code>count</code>, <code>real</code>, <code>flag</code>, and <code>symbol</code></li>
        <li>Control flow with <code>verify</code> / <code>else_verify</code> / <code>otherwise</code></li>
        <li>Loops: <code>orbit</code> (while), <code>orbit while</code>, and <code>orbit times</code> (for)</li>
        <li>Functions via <code>command</code> definitions</li>
        <li>Arrays and string support</li>
        <li>Three-address code generation with optimization</li>
        <li>Built-in runtime interpreter</li>
      </ul>
    </article>
  );
}
