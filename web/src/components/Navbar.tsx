import Link from "next/link";

const navLinks = [
  { label: "Playground", href: "/playground" },
  { label: "Docs", href: "/docs" },
  { label: "Language", href: "/docs/syntax" },
  { label: "GitHub", href: "https://github.com/Rockstatata/AstroScript", external: true },
];

function RocketIcon() {
  return (
    <svg
      aria-hidden="true"
      viewBox="0 0 24 24"
      className="h-5 w-5 text-primary"
      fill="none"
      stroke="currentColor"
      strokeWidth="1.8"
    >
      <path d="M8.4 15.6c-1.5-3.2.4-8.2 4.8-11.1 4.4 2.9 6.3 7.9 4.8 11.1-2 1.9-4.5 2.9-4.8 3-.3-.1-2.8-1.1-4.8-3Z" />
      <path d="M5 19c1.5-.2 2.8-1 3.8-2.3M11.2 10.5l2.3 2.3" />
      <circle cx="14.7" cy="8.9" r="1.1" />
      <path d="M5.2 14.2 3.8 16.9l2.7-1.4M10.8 19.8l-1.4 2.7 2.7-1.4" />
    </svg>
  );
}

export default function Navbar() {
  return (
    <header className="sticky top-0 z-50 border-b border-primary/10 bg-black/40 backdrop-blur-xl">
      <nav className="mx-auto flex h-16 w-full max-w-[1240px] items-center justify-between px-5 md:px-8">
        <Link href="/" className="inline-flex items-center gap-2 text-[27px] font-semibold leading-none text-white">
          <RocketIcon />
          <span className="text-[28px]">AstroScript</span>
        </Link>

        <ul className="hidden items-center gap-9 text-sm font-semibold text-white/90 md:flex">
          {navLinks.map((item) => (
            <li key={item.label}>
              <Link
                href={item.href}
                target={item.external ? "_blank" : undefined}
                rel={item.external ? "noreferrer" : undefined}
                className="transition-colors hover:text-white"
              >
                {item.label}
              </Link>
            </li>
          ))}
        </ul>

        <Link
          href="/playground"
          className="rounded-md border border-primary/40 bg-primary px-5 py-2 text-sm font-semibold text-white shadow-[0_0_20px_rgba(44,44,226,0.4)] transition-all hover:scale-[1.02] hover:bg-[#3a3af0]"
        >
          Try Playground
        </Link>
      </nav>
    </header>
  );
}
