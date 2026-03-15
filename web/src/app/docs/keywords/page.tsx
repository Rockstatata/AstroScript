const sections: Array<{ title: string; rows: string[][] }> = [
  {
    title: "Program Structure",
    rows: [
      ["mission", "Program entry point", "main()"],
      ["launch", "Begin mission block", "main body"],
      ["success", "Successful termination", "return 0"],
      ["abort", "Abnormal termination", "exit(1)"],
    ],
  },
  {
    title: "Types",
    rows: [
      ["count", "Integer type", "int"],
      ["real", "Floating-point type", "float"],
      ["precise", "Double precision", "double"],
      ["flag", "Boolean signal", "bool"],
      ["symbol", "Character type", "char"],
      ["voidspace", "No return value", "void"],
    ],
  },
  {
    title: "Control Flow",
    rows: [
      ["verify", "Conditional check", "if"],
      ["else_verify", "Alternate condition", "else if"],
      ["otherwise", "Fallback branch", "else"],
      ["orbit", "Loop construct", "while"],
      ["stage_sep", "Exit loop", "break"],
      ["coast", "Skip iteration", "continue"],
    ],
  },
  {
    title: "Functions and Modules",
    rows: [
      ["command", "Define a function", "function"],
      ["back", "Return from function", "return"],
      ["module", "Class-like module", "class"],
      ["deploy", "Instantiate module", "new"],
      ["extends", "Inheritance", "extends"],
      ["this", "Current instance", "this"],
    ],
  },
];

export default function DocsKeywords() {
  return (
    <article className="text-white/90">
      <nav aria-label="Breadcrumb" className="mb-7 text-sm text-white/45">
        <ol className="flex items-center gap-2">
          <li>Docs</li>
          <li className="text-white/30">&gt;</li>
          <li className="text-white/75">Keywords</li>
        </ol>
      </nav>

      <header>
        <h1 className="text-5xl font-extrabold tracking-tight text-white sm:text-6xl">Language Keywords</h1>
        <p className="mt-6 max-w-3xl text-[1.08rem] leading-8 text-white/72">
          AstroScript maps mission vocabulary to familiar compiler constructs. This table is your
          quick reference when translating C/C++ ideas into AstroScript grammar.
        </p>
      </header>

      <div className="mt-10 space-y-10">
        {sections.map((section) => (
          <section key={section.title}>
            <h2 className="mb-4 text-3xl font-semibold tracking-tight text-white">{section.title}</h2>
            <div className="overflow-x-auto rounded-xl border border-white/10 bg-[#12162f]/70">
              <table className="w-full min-w-155 text-left text-sm">
                <thead className="bg-[#1b2142] text-white/75">
                  <tr>
                    <th className="px-4 py-3">Keyword</th>
                    <th className="px-4 py-3">Purpose</th>
                    <th className="px-4 py-3">C/C++ Equivalent</th>
                  </tr>
                </thead>
                <tbody>
                  {section.rows.map((row) => (
                    <tr key={row[0]} className="border-t border-white/8 text-white/70">
                      <td className="px-4 py-3 font-mono text-primary">{row[0]}</td>
                      <td className="px-4 py-3">{row[1]}</td>
                      <td className="px-4 py-3 font-mono">{row[2]}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </section>
        ))}

        <section>
          <h2 className="mb-4 text-3xl font-semibold tracking-tight text-white">Math Built-ins</h2>
          <p className="rounded-xl border border-white/10 bg-[#12162f]/70 px-4 py-4 font-mono text-sm leading-7 text-primary/90">
            root, flr, ceil, abs, logarithm, sine, cosine, tan, asine, acosine, atan, prime
          </p>
        </section>
      </div>
    </article>
  );
}
