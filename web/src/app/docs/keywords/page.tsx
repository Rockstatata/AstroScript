export default function DocsKeywords() {
  return (
    <article>
      <h1>Keywords Reference</h1>

      <h2>Program Structure</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>mission</code></td><td>Program entry point</td><td><code>main()</code></td></tr>
          <tr><td><code>launch</code></td><td>Begin mission block</td><td>start of main</td></tr>
          <tr><td><code>success</code></td><td>End program</td><td><code>return 0</code></td></tr>
          <tr><td><code>abort</code></td><td>Terminate execution</td><td><code>exit(1)</code></td></tr>
        </tbody>
      </table>

      <h2>Types</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>count</code></td><td>Integer type</td><td><code>int</code></td></tr>
          <tr><td><code>real</code></td><td>Float type</td><td><code>float</code></td></tr>
          <tr><td><code>precise</code></td><td>Double precision</td><td><code>double</code></td></tr>
          <tr><td><code>flag</code></td><td>Boolean type</td><td><code>bool</code></td></tr>
          <tr><td><code>symbol</code></td><td>String/char type</td><td><code>string</code></td></tr>
          <tr><td><code>voidspace</code></td><td>Void type</td><td><code>void</code></td></tr>
        </tbody>
      </table>

      <h2>Declarations</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th></tr></thead>
        <tbody>
          <tr><td><code>telemetry</code></td><td>Variable declaration</td></tr>
          <tr><td><code>limit</code></td><td>Constant declaration</td></tr>
          <tr><td><code>alias</code></td><td>Type alias</td></tr>
          <tr><td><code>fleet</code></td><td>Collection declaration</td></tr>
        </tbody>
      </table>

      <h2>Control Flow</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>verify</code></td><td>If statement</td><td><code>if</code></td></tr>
          <tr><td><code>else_verify</code></td><td>Else-if</td><td><code>else if</code></td></tr>
          <tr><td><code>otherwise</code></td><td>Else</td><td><code>else</code></td></tr>
          <tr><td><code>scenario</code></td><td>Switch statement</td><td><code>switch</code></td></tr>
          <tr><td><code>trajectory</code></td><td>Case label</td><td><code>case</code></td></tr>
          <tr><td><code>fallback</code></td><td>Default case</td><td><code>default</code></td></tr>
        </tbody>
      </table>

      <h2>Loops</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>orbit</code></td><td>Loop construct</td><td><code>while</code></td></tr>
          <tr><td><code>while</code></td><td>While modifier</td><td><code>while</code></td></tr>
          <tr><td><code>times</code></td><td>For-style loop</td><td><code>for</code></td></tr>
          <tr><td><code>stage_sep</code></td><td>Break loop</td><td><code>break</code></td></tr>
          <tr><td><code>coast</code></td><td>Continue loop</td><td><code>continue</code></td></tr>
        </tbody>
      </table>

      <h2>Functions</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>command</code></td><td>Function definition</td><td><code>function</code></td></tr>
          <tr><td><code>back</code></td><td>Return statement</td><td><code>return</code></td></tr>
        </tbody>
      </table>

      <h2>I/O</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>transmit</code></td><td>Print output</td><td><code>printf</code></td></tr>
          <tr><td><code>broadcast</code></td><td>Broadcast output</td><td><code>println</code></td></tr>
          <tr><td><code>receive</code></td><td>Read input</td><td><code>scanf</code></td></tr>
          <tr><td><code>alarm</code></td><td>Error output</td><td><code>stderr</code></td></tr>
        </tbody>
      </table>

      <h2>OOP</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Purpose</th><th>C/C++ Equivalent</th></tr></thead>
        <tbody>
          <tr><td><code>module</code></td><td>Class definition</td><td><code>class</code></td></tr>
          <tr><td><code>deploy</code></td><td>Instantiation</td><td><code>new</code></td></tr>
          <tr><td><code>extends</code></td><td>Inheritance</td><td><code>extends</code></td></tr>
          <tr><td><code>public</code></td><td>Public access</td><td><code>public</code></td></tr>
          <tr><td><code>private</code></td><td>Private access</td><td><code>private</code></td></tr>
          <tr><td><code>this</code></td><td>Self-reference</td><td><code>this</code></td></tr>
        </tbody>
      </table>

      <h2>Operators</h2>
      <table>
        <thead><tr><th>Keyword</th><th>Symbol</th><th>Operation</th></tr></thead>
        <tbody>
          <tr><td><code>add</code></td><td><code>+</code></td><td>Addition</td></tr>
          <tr><td><code>minus</code></td><td><code>-</code></td><td>Subtraction</td></tr>
          <tr><td><code>mul</code></td><td><code>*</code></td><td>Multiplication</td></tr>
          <tr><td><code>divide</code></td><td><code>/</code></td><td>Division</td></tr>
          <tr><td><code>mod</code></td><td></td><td>Modulo</td></tr>
          <tr><td></td><td><code>**</code></td><td>Power</td></tr>
          <tr><td></td><td><code>:=</code></td><td>Assignment</td></tr>
          <tr><td></td><td><code>:-&gt;</code></td><td>Return type</td></tr>
          <tr><td><code>AND</code></td><td></td><td>Logical AND</td></tr>
          <tr><td><code>OR</code></td><td></td><td>Logical OR</td></tr>
          <tr><td><code>NOT</code></td><td></td><td>Logical NOT</td></tr>
          <tr><td><code>XOR</code></td><td></td><td>Logical XOR</td></tr>
        </tbody>
      </table>

      <h2>Math Built-ins</h2>
      <p>
        <code>root</code>, <code>flr</code>, <code>ceil</code>, <code>abs</code>,{" "}
        <code>logarithm</code>, <code>sine</code>, <code>cosine</code>, <code>tan</code>,{" "}
        <code>asine</code>, <code>acosine</code>, <code>atan</code>, <code>prime</code>
      </p>
    </article>
  );
}
