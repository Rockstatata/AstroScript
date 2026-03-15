export default function DocsSyntax() {
  return (
    <article>
      <h1>Language Syntax</h1>

      <h2>Program Structure</h2>
      <p>Every AstroScript program must be wrapped in a mission block:</p>
      <pre><code>{`mission ProgramName launch {
    $$ Your code here
} success`}</code></pre>

      <h2>Statements</h2>
      <p>All statements end with a period (<code>.</code>), which acts as the statement terminator.</p>

      <h2>Variable Declaration</h2>
      <p>Use <code>telemetry</code> to declare variables:</p>
      <pre><code>{`telemetry count x.
telemetry real pi.
telemetry symbol name.
telemetry flag isReady.`}</code></pre>

      <p>Declare with initialization:</p>
      <pre><code>{`telemetry count x := 10.`}</code></pre>

      <p>Use <code>limit</code> for constants:</p>
      <pre><code>{`limit count MAX := 100.`}</code></pre>

      <h2>Assignment</h2>
      <p>Use <code>:=</code> for assignment:</p>
      <pre><code>{`x := 42.
y := x add 10.`}</code></pre>

      <h2>Arithmetic</h2>
      <p>AstroScript supports both keyword and symbol operators:</p>
      <table>
        <thead>
          <tr><th>Operation</th><th>Keyword</th><th>Symbol</th></tr>
        </thead>
        <tbody>
          <tr><td>Addition</td><td><code>add</code></td><td><code>+</code></td></tr>
          <tr><td>Subtraction</td><td><code>minus</code></td><td><code>-</code></td></tr>
          <tr><td>Multiplication</td><td><code>mul</code></td><td><code>*</code></td></tr>
          <tr><td>Division</td><td><code>divide</code></td><td><code>/</code></td></tr>
          <tr><td>Modulo</td><td><code>mod</code></td><td></td></tr>
          <tr><td>Power</td><td></td><td><code>**</code></td></tr>
        </tbody>
      </table>

      <h2>Comparison Operators</h2>
      <p><code>&lt;</code>, <code>&gt;</code>, <code>&lt;=</code>, <code>&gt;=</code>, <code>==</code>, <code>!=</code></p>

      <h2>Logical Operators</h2>
      <p><code>AND</code>, <code>OR</code>, <code>NOT</code>, <code>XOR</code></p>

      <h2>Conditionals</h2>
      <pre><code>{`verify (x > 0) {
    transmit x.
} else_verify (x == 0) {
    transmit 0.
} otherwise {
    transmit -1.
}`}</code></pre>

      <h2>Loops</h2>
      <p>While loop:</p>
      <pre><code>{`orbit (counter < 10) {
    transmit counter.
    counter := counter add 1.
}`}</code></pre>

      <p>For-style loop:</p>
      <pre><code>{`orbit times (i : 0 : 10) {
    transmit i.
}`}</code></pre>

      <h2>Functions</h2>
      <pre><code>{`command add(count a, count b) : count {
    back a add b.
}`}</code></pre>

      <h2>Input / Output</h2>
      <pre><code>{`transmit expression.    $$ print
receive variable.       $$ read input`}</code></pre>

      <h2>Comments</h2>
      <pre><code>{`$$ This is a single-line comment

$* This is a
   multi-line comment *$`}</code></pre>

      <h2>Arrays</h2>
      <pre><code>{`telemetry count arr[5].
arr[0] := 10.
transmit arr[0].`}</code></pre>
    </article>
  );
}
