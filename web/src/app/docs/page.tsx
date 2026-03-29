import DocsCallout from "@/components/docs/DocsCallout";
import DocsCodeBlock from "@/components/docs/DocsCodeBlock";

const quickStartCode = `# macOS / Linux
./scripts/build_compiler.sh
./backend/compiler/build/astroscript examples/comprehensive.as

# Windows (PowerShell)
.\\scripts\\build_compiler.ps1
.\\backend\\compiler\\build\\astroscript.exe .\\examples\\comprehensive.as

# Start docs and playground
cd web
npm install
npm run dev`;

const astroExample = `mission Apollo launch
{
    telemetry count astronauts := 3.

    verify (astronauts > 0)
    {
      transmit "Mission ready".
    }

    orbit (astronauts > 0)
    {
      transmit astronauts.
        astronauts := astronauts minus 1.
    }
}
success`;

export default function DocsIntroduction() {
  return (
    <article className="text-white/90">
      <nav aria-label="Breadcrumb" className="mb-7 text-sm text-white/45">
        <ol className="flex items-center gap-2">
          <li>Docs</li>
          <li className="text-white/30">&gt;</li>
          <li className="text-white/75">Introduction</li>
        </ol>
      </nav>

      <header>
        <h1 className="text-5xl font-extrabold tracking-tight text-white sm:text-6xl">Introduction</h1>
        <p className="mt-6 max-w-3xl text-[1.2rem] leading-9 text-white/72">
          Welcome to the official AstroScript documentation. AstroScript is a mission-oriented
          programming language implemented using Flex and Bison, designed to power practical
          mission planning and management workflows while illustrating compiler construction
          concepts from lexical analysis to optimized intermediate code.
        </p>
      </header>

      <DocsCallout title="Did you know?">
        <p>
          AstroScript replaces traditional programming constructs with space-themed keywords such as
          <span className="mx-1 font-mono text-primary">mission</span>,
          <span className="mx-1 font-mono text-primary">launch</span>,
          <span className="mx-1 font-mono text-primary">telemetry</span>,
          <span className="mx-1 font-mono text-primary">verify</span>, and
          <span className="ml-1 font-mono text-primary">orbit</span>.
          These map directly to common constructs like main(), variable declarations, conditionals,
          and loops, while newer keywords like
          <span className="mx-1 font-mono text-primary">override</span>,
          <span className="mx-1 font-mono text-primary">super</span>, and
          <span className="ml-1 font-mono text-primary">new</span>
          support advanced mission-architecture patterns.
        </p>
      </DocsCallout>

      <section id="what-is-astroscript" className="mt-12 scroll-mt-24">
        <h2 className="text-4xl font-bold tracking-tight text-white">What is AstroScript?</h2>
        <div className="mt-3 h-px bg-white/10" />
        <p className="mt-5 text-lg leading-8 text-white/72">
          AstroScript is a space mission-based programming language where a program begins with a
          mission declaration and launch phase, and ends with either success or abort. It is built
          for students, compiler enthusiasts, and builders who want to implement real mission
          management systems using typed functions, modules, inheritance, overloading, and scoped
          state transitions.
        </p>
      </section>

      <section id="core-philosophy" className="mt-8 scroll-mt-24">
        <h3 className="text-2xl font-semibold tracking-tight text-white">Core Philosophy</h3>
        <p className="mt-4 text-lg leading-8 text-white/72">
          The language focuses on static typing, mission-based structure, compiler pipeline
          visualization, and educational compiler design. Every keyword and grammar construct is
          purposefully shaped to make lexical, syntactic, and semantic phases easier to reason
          about.
        </p>

        <div className="mt-8 grid gap-4 sm:grid-cols-2">
          <article className="rounded-xl border border-white/10 bg-slate-900/50 p-5">
            <h4 className="text-2xl font-semibold text-white">Zero-Cost Abstractions</h4>
            <p className="mt-2 text-sm leading-7 text-white/65">
              High-level mission syntax compiles down to efficient semantic actions and optimized
              three-address code.
            </p>
          </article>

          <article className="rounded-xl border border-white/10 bg-slate-900/50 p-5">
            <h4 className="text-2xl font-semibold text-white">Inspectable Compiler Pipeline</h4>
            <p className="mt-2 text-sm leading-7 text-white/65">
              Each stage, from tokenization to TAC execution, is intentionally transparent for
              debugging and learning compiler internals.
            </p>
          </article>
        </div>
      </section>

      <section id="quick-start" className="mt-14 scroll-mt-24">
        <h2 className="text-4xl font-bold tracking-tight text-white">Quick Start</h2>
        <p className="mt-4 text-lg leading-8 text-white/72">
          Install the compiler toolchain and run your first AstroScript mission locally.
        </p>
        <div className="mt-5">
          <DocsCodeBlock filename="shell" language="shell" code={quickStartCode} />
        </div>
      </section>

      <section id="simple-example" className="mt-14 scroll-mt-24">
        <h2 className="text-4xl font-bold tracking-tight text-white">A Simple Example</h2>
        <p className="mt-4 text-lg leading-8 text-white/72">
          This sample shows a complete mission lifecycle with declaration, verification, and orbital
          iteration.
        </p>
        <div className="mt-5">
          <DocsCodeBlock filename="main.astro" language="astroscript" code={astroExample} />
        </div>
        <ul className="mt-5 space-y-2 text-sm leading-7 text-white/65">
          <li>
            <span className="font-mono text-primary">mission</span> maps to the main program entry.
          </li>
          <li>
            <span className="font-mono text-primary">telemetry</span> declares statically typed
            mission variables.
          </li>
          <li>
            <span className="font-mono text-primary">verify</span> represents conditional branching.
          </li>
          <li>
            <span className="font-mono text-primary">orbit</span> models iterative loop execution.
          </li>
        </ul>
      </section>

      <section id="community-support" className="mt-14 scroll-mt-24">
        <h2 className="text-3xl font-bold tracking-tight text-white">Community Support</h2>
        <p className="mt-4 max-w-3xl text-base leading-8 text-white/68">
          AstroScript is a learning-focused compiler ecosystem. Join the community to discuss
          keyword design, parser decisions, and compiler pipeline enhancements.
        </p>
      </section>
    </article>
  );
}
