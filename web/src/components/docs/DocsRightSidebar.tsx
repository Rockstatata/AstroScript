import Link from "next/link";

const links = [
  { href: "#what-is-astroscript", label: "What is AstroScript?" },
  { href: "#core-philosophy", label: "Core Philosophy" },
  { href: "#quick-start", label: "Quick Start" },
  { href: "#simple-example", label: "A Simple Example" },
  { href: "#community-support", label: "Community Support" },
];

export default function DocsRightSidebar() {
  return (
    <aside
      className="sticky top-16 hidden h-[calc(100vh-4rem)] w-[270px] shrink-0 px-5 py-8 xl:block"
      aria-label="On this page"
    >
      <div className="space-y-6">
        <section>
          <h2 className="mb-3 text-[11px] font-semibold uppercase tracking-[0.18em] text-white/35">On this page</h2>
          <nav>
            <ul className="space-y-2">
              {links.map((link) => (
                <li key={link.href}>
                  <Link
                    href={link.href}
                    className="text-sm text-white/55 transition-colors duration-200 hover:text-primary"
                  >
                    {link.label}
                  </Link>
                </li>
              ))}
            </ul>
          </nav>
        </section>

        <section className="rounded-lg border border-white/10 bg-[#131a38]/70 p-4">
          <h3 className="text-sm font-semibold text-white">Need help?</h3>
          <p className="mt-1 text-sm leading-relaxed text-white/60">
            Join our Discord community and get answers in real-time.
          </p>
          <Link
            href="https://discord.com"
            target="_blank"
            rel="noreferrer"
            className="mt-3 inline-flex items-center gap-1 text-sm font-medium text-primary transition-colors hover:text-[#6f6ff2]"
          >
            Join Discord
            <span aria-hidden="true">&gt;</span>
          </Link>
        </section>
      </div>
    </aside>
  );
}
