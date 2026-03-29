import DocsCodeBlock from "@/components/docs/DocsCodeBlock";
import languageRef from "@/lib/language-reference.generated.json";

export default function DocsSyntax() {
  return (
    <article className="text-white/90">
      <nav aria-label="Breadcrumb" className="mb-7 text-sm text-white/45">
        <ol className="flex items-center gap-2">
          <li>Docs</li>
          <li className="text-white/30">&gt;</li>
          <li className="text-white/75">Language Basics</li>
        </ol>
      </nav>

      <header>
        <h1 className="text-5xl font-extrabold tracking-tight text-white sm:text-6xl">Language Basics</h1>
        <p className="mt-6 max-w-3xl text-[1.08rem] leading-8 text-white/72">
          AstroScript syntax is mission-oriented by design. Programs begin with mission launch,
          use period-terminated statements, and express control flow using readable space-themed
          keywords.
        </p>
        <p className="mt-3 max-w-3xl text-[1.02rem] leading-7 text-white/65">
          In the playground, each compile now exposes optimized IR and a readable C Translation tab
          so learners can relate AstroScript flow to familiar C-style statements.
        </p>
      </header>

      <section className="mt-10 space-y-10">
        <div id="program-structure" className="scroll-mt-24">
          <h2 className="text-3xl font-semibold tracking-tight text-white">Program Structure</h2>
          <p className="mt-3 text-white/70">Every AstroScript program is wrapped in a mission block:</p>
          <div className="mt-4">
            <DocsCodeBlock
              filename="program.astro"
              language="astroscript"
              code={`mission ProgramName launch
{
    $$ Your code here
}
success`}
            />
          </div>
        </div>

        <div id="declarations-and-assignment" className="scroll-mt-24">
          <h2 className="text-3xl font-semibold tracking-tight text-white">Declarations and Assignment</h2>
          <p className="mt-3 text-white/70">
            Use telemetry for variables, limit for constants, and := for assignment. Statements end
            with a period.
          </p>
          <div className="mt-4">
            <DocsCodeBlock
              filename="declarations.astro"
              language="astroscript"
              code={`telemetry count x.
telemetry real pi.
telemetry symbol name.
telemetry flag isReady.
limit count MAX := 100.

x := 42.
pi := 3.14.`}
            />
          </div>
        </div>

        <div id="conditionals-and-loops" className="scroll-mt-24">
          <h2 className="text-3xl font-semibold tracking-tight text-white">Conditionals and Loops</h2>
          <div className="mt-4">
            <DocsCodeBlock
              filename="control-flow.astro"
              language="astroscript"
              code={`verify (x > 0)
{
    transmit x.
}
else_verify (x == 0)
{
    transmit 0.
}
otherwise
{
    transmit -1.
}

orbit (x < 10)
{
    x := x add 1.
}`}
            />
          </div>
        </div>

        <div id="functions-and-io" className="scroll-mt-24">
          <h2 className="text-3xl font-semibold tracking-tight text-white">Functions and I/O</h2>
          <div className="mt-4">
            <DocsCodeBlock
              filename="functions.astro"
              language="astroscript"
              code={`command add(count a, count b) : count
{
    back a add b.
}

transmit expression.
receive variable.`}
            />
          </div>
          <p className="mt-3 text-white/70">
            AstroScript also accepts <span className="font-mono text-primary">{":->"}</span> as a return-type separator.
          </p>
        </div>

        <div id="overloading-and-scope" className="scroll-mt-24">
          <h2 className="text-3xl font-semibold tracking-tight text-white">Overloading and Scoped Variables</h2>
          <p className="mt-3 text-white/70">
            AstroScript supports function and method overloading by parameter count, plus lexical
            variable shadowing for mission blocks.
          </p>
          <div className="mt-4">
            <DocsCodeBlock
              filename="overloading.astro"
              language="astroscript"
              code={`command allocate(count slots) : count
{
    back slots add 1.
}

command allocate(count slots, count reserve) : count
{
    back slots add reserve.
}

module MissionLog
{
    command push(symbol entry) : count
    {
        back 1.
    }

    command push(symbol entry, count priority) : count
    {
        back priority.
    }
}

telemetry count cap := allocate(3).
telemetry count cap2 := allocate(3, 2).

verify (1 == 1)
{
    telemetry count cap := 99.
    transmit cap.
}

transmit cap.`}
            />
          </div>
        </div>

        <div id="mission-planning-oop" className="scroll-mt-24">
          <h2 className="text-3xl font-semibold tracking-tight text-white">Mission Planning OOP Pattern</h2>
          <div className="mt-4">
            <DocsCodeBlock
              filename="mission-planner.astro"
              language="astroscript"
              code={`module MissionPlan
{
    public telemetry count tasks.

    command MissionPlan(count initialTasks) : voidspace
    {
        this.tasks := initialTasks.
    }

    command schedule(count incoming) : count
    {
        this.tasks := this.tasks add incoming.
        back this.tasks.
    }
}

module PriorityMissionPlan extends MissionPlan
{
    public telemetry count emergencyBoost := 2.

    command PriorityMissionPlan(count initialTasks) : voidspace
    {
        this.tasks := initialTasks.
    }

    override command schedule(count incoming) : count
    {
        back super.schedule(incoming add this.emergencyBoost).
    }
}

deploy PriorityMissionPlan planner(8).
transmit planner.schedule(3).
transmit planner.tasks.`}
            />
          </div>
        </div>

        <div className="overflow-x-auto rounded-xl border border-white/10 bg-[#12162f]/70">
          <table className="w-full min-w-155 text-left text-sm">
            <thead className="bg-[#1b2142] text-white/75">
              <tr>
                <th className="px-4 py-3">Operation</th>
                <th className="px-4 py-3">Keyword</th>
                <th className="px-4 py-3">Symbol</th>
              </tr>
            </thead>
            <tbody>
              {languageRef.operators.arithmetic.map((row) => (
                <tr key={row.operation} className="border-t border-white/8 text-white/70">
                  <td className="px-4 py-3">{row.operation}</td>
                  <td className="px-4 py-3 font-mono text-primary">{row.keyword}</td>
                  <td className="px-4 py-3 font-mono text-primary">{row.symbol}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>

        <div className="grid gap-4 lg:grid-cols-2">
          <article className="rounded-xl border border-white/10 bg-[#12162f]/70 p-4">
            <h3 className="text-lg font-semibold text-white">Comparison Operators</h3>
            <p className="mt-3 font-mono text-sm text-primary">{languageRef.operators.comparisons.join(", ")}</p>
          </article>

          <article className="rounded-xl border border-white/10 bg-[#12162f]/70 p-4">
            <h3 className="text-lg font-semibold text-white">Logical Operators</h3>
            <p className="mt-3 font-mono text-sm text-primary">{languageRef.operators.logical.join(", ")}</p>
          </article>
        </div>

        <p className="rounded-xl border border-white/10 bg-[#12162f]/70 px-4 py-3 text-sm leading-7 text-white/75">
          {languageRef.notes.reservedMeaning}
        </p>

        <div>
          <h2 className="text-3xl font-semibold tracking-tight text-white">Orbit Times Loop</h2>
          <div className="mt-4">
            <DocsCodeBlock
              filename="orbit-times.astro"
              language="astroscript"
              code={`telemetry count i.

orbit times (i : 0 : 5)
{
    transmit i.
}`}
            />
          </div>
        </div>
      </section>
    </article>
  );
}
