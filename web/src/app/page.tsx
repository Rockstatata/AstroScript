import Link from "next/link";

export default function Home() {
  return (
    <div className="min-h-screen bg-gradient-to-b from-gray-950 to-gray-900 text-white">
      <nav className="flex items-center justify-between px-8 py-4 border-b border-gray-800">
        <span className="text-xl font-bold tracking-tight">AstroScript</span>
        <div className="flex gap-6 text-sm">
          <Link href="/playground" className="hover:text-blue-400 transition-colors">
            Playground
          </Link>
          <Link href="/docs" className="hover:text-blue-400 transition-colors">
            Docs
          </Link>
        </div>
      </nav>

      <main className="flex flex-col items-center justify-center px-8 py-32 text-center">
        <h1 className="text-5xl font-bold tracking-tight sm:text-6xl">
          Astro<span className="text-blue-400">Script</span>
        </h1>
        <p className="mt-6 max-w-xl text-lg text-gray-400">
          A space-themed programming language with a custom compiler built using
          Flex, Bison, and C++. Write missions, launch programs, and explore the cosmos of code.
        </p>

        <div className="mt-10 flex gap-4">
          <Link
            href="/playground"
            className="rounded-lg bg-blue-600 px-6 py-3 font-medium transition-colors hover:bg-blue-500"
          >
            Try Playground
          </Link>
          <Link
            href="/docs"
            className="rounded-lg border border-gray-700 px-6 py-3 font-medium transition-colors hover:bg-gray-800"
          >
            Read Docs
          </Link>
        </div>

        <div className="mt-20 grid max-w-3xl grid-cols-1 gap-8 sm:grid-cols-3">
          <div className="rounded-lg border border-gray-800 p-6 text-left">
            <h3 className="font-semibold text-blue-400">Custom Compiler</h3>
            <p className="mt-2 text-sm text-gray-400">
              Lexer, parser, semantic analysis, and intermediate code generation
              with optimization.
            </p>
          </div>
          <div className="rounded-lg border border-gray-800 p-6 text-left">
            <h3 className="font-semibold text-blue-400">Web Playground</h3>
            <p className="mt-2 text-sm text-gray-400">
              Write and run AstroScript code directly in your browser with
              Monaco Editor.
            </p>
          </div>
          <div className="rounded-lg border border-gray-800 p-6 text-left">
            <h3 className="font-semibold text-blue-400">Space Themed</h3>
            <p className="mt-2 text-sm text-gray-400">
              Missions, launches, orbits, and telemetry — programming with a
              cosmic vocabulary.
            </p>
          </div>
        </div>
      </main>
    </div>
  );
}
