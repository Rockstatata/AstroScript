const cards = [
  {
    title: "Designed for Learning Compilers",
    description:
      "A clean architecture designed to teach the fundamentals of compiler design through modular construction.",
    icon: (
      <svg viewBox="0 0 24 24" className="h-5 w-5" fill="none" stroke="currentColor" strokeWidth="1.8">
        <path d="M4 7.5 12 4l8 3.5L12 11 4 7.5Z" />
        <path d="M4 12.5 12 16l8-3.5" />
      </svg>
    ),
  },
  {
    title: "Flex + Bison Powered",
    description:
      "Leveraging industry-standard tools for robust lexical and syntactic analysis that stands the test of time.",
    icon: (
      <svg viewBox="0 0 24 24" className="h-5 w-5" fill="none" stroke="currentColor" strokeWidth="1.8">
        <rect x="6" y="6" width="12" height="12" rx="2" />
        <path d="M9 3v3M15 3v3M9 18v3M15 18v3M3 9h3M18 9h3M3 15h3M18 15h3" />
      </svg>
    ),
  },
  {
    title: "Space Inspired Syntax",
    description:
      "A unique DSL that makes coding feel like navigating the stars with specialized primitives for telemetry.",
    icon: (
      <svg viewBox="0 0 24 24" className="h-5 w-5" fill="none" stroke="currentColor" strokeWidth="1.8">
        <path d="m12 3 1.7 3.6L18 8.2l-3 2.9.8 4.2L12 13.5l-3.8 1.8.8-4.2-3-2.9 4.3-1.6L12 3Z" />
      </svg>
    ),
  },
];

export default function FeatureCards() {
  return (
    <section className="mx-auto w-full max-w-[1240px] border-t border-white/8 px-5 py-24 lg:px-8">
      <h2 className="text-center text-5xl font-bold tracking-[-0.02em] text-white">Engineered for the Void</h2>
      <p className="mx-auto mt-6 max-w-[760px] text-center text-lg leading-relaxed text-white/65 md:text-[1.35rem]">
        Robust architecture meeting the demands of high-concurrency interstellar computing.
      </p>

      <div className="mt-16 grid gap-5 md:grid-cols-3">
        {cards.map((card) => (
          <article
            key={card.title}
            className="rounded-xl border border-primary/20 bg-black/30 p-8 backdrop-blur-md transition-all hover:scale-[1.02] hover:border-primary/50 hover:shadow-[0_0_24px_rgba(44,44,226,0.22)]"
          >
            <div className="mb-6 inline-flex h-12 w-12 items-center justify-center rounded-lg border border-primary/30 bg-primary/15 text-primary">
              {card.icon}
            </div>
            <h3 className="text-[1.95rem] font-semibold leading-tight text-white">{card.title}</h3>
            <p className="mt-4 text-base leading-relaxed text-white/66">{card.description}</p>
          </article>
        ))}
      </div>
    </section>
  );
}
