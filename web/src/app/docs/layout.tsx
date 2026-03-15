import Link from "next/link";

const docLinks = [
  { href: "/docs", label: "Introduction" },
  { href: "/docs/syntax", label: "Language Syntax" },
  { href: "/docs/keywords", label: "Keywords" },
  { href: "/docs/architecture", label: "Compiler Architecture" },
];

export default function DocsLayout({ children }: { children: React.ReactNode }) {
  return (
    <div className="min-h-screen bg-gray-950 text-white">
      <nav className="flex items-center justify-between border-b border-gray-800 px-6 py-3">
        <Link href="/" className="text-lg font-bold tracking-tight">
          AstroScript
        </Link>
        <div className="flex items-center gap-4 text-sm">
          <Link href="/playground" className="text-gray-400 hover:text-white transition-colors">
            Playground
          </Link>
        </div>
      </nav>

      <div className="flex max-w-6xl mx-auto">
        <aside className="w-56 shrink-0 border-r border-gray-800 p-6">
          <h2 className="text-xs font-semibold uppercase tracking-wider text-gray-500 mb-4">
            Documentation
          </h2>
          <ul className="space-y-2">
            {docLinks.map((link) => (
              <li key={link.href}>
                <Link
                  href={link.href}
                  className="block text-sm text-gray-400 hover:text-white transition-colors"
                >
                  {link.label}
                </Link>
              </li>
            ))}
          </ul>
        </aside>

        <main className="flex-1 p-8 prose prose-invert prose-sm max-w-none">
          {children}
        </main>
      </div>
    </div>
  );
}
