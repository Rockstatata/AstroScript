import Link from "next/link";

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

const links = [
  { label: "GitHub", href: "https://github.com/Rockstatata/AstroScript", external: true },
  { label: "Playground", href: "/playground" },
  { label: "Docs", href: "/docs" },
  { label: "About", href: "/docs/architecture" },
];

export default function Footer() {
  return (
    <footer className="border-t border-white/8">
      <div className="mx-auto flex w-full max-w-[1240px] flex-col items-center justify-between gap-6 px-5 py-10 md:flex-row md:px-8">
        <div className="inline-flex items-center gap-2 text-[27px] font-semibold text-white">
          <RocketIcon />
          <span>AstroScript</span>
        </div>

        <nav aria-label="Footer" className="flex flex-wrap items-center justify-center gap-8 text-sm text-white/70">
          {links.map((item) => (
            <Link
              key={item.label}
              href={item.href}
              target={item.external ? "_blank" : undefined}
              rel={item.external ? "noreferrer" : undefined}
              className="transition-colors hover:text-white"
            >
              {item.label}
            </Link>
          ))}
        </nav>

        <p className="text-xs text-white/40">© 2024 AstroScript Foundation. Licensed under MIT.</p>
      </div>
    </footer>
  );
}
