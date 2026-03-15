import Link from "next/link";
import CodePreview from "@/components/CodePreview";

export default function HeroSection() {
  return (
    <section className="mx-auto grid w-full max-w-[1240px] grid-cols-1 gap-14 px-5 pb-28 pt-14 lg:grid-cols-2 lg:items-start lg:px-8 lg:pt-16">
      <div>
        <div className="mb-8 inline-flex items-center rounded-full border border-primary/30 bg-primary/10 px-4 py-1.5 text-xs font-semibold tracking-[0.18em] text-[#7782ff]">
          V1.0.0 NOW AVAILABLE
        </div>

        <h1 className="max-w-[540px] text-5xl font-extrabold leading-[0.95] tracking-[-0.02em] text-white sm:text-6xl md:text-7xl">
          <span className="block">Explore Programming</span>
          <span className="mt-1 block bg-gradient-to-b from-[#4a5ef9] to-[#5b96ff] bg-clip-text text-transparent">
            Through the Cosmos
          </span>
        </h1>

        <p className="mt-8 max-w-[560px] text-lg leading-relaxed text-white/72 md:text-[1.32rem] md:leading-relaxed">
          A professional developer tool built with Flex + Bison for the next generation of space-faring code.
          High performance, stellar syntax, and astronomical efficiency.
        </p>

        <div className="mt-10 flex flex-wrap gap-4">
          <Link
            href="/playground"
            className="rounded-xl border border-primary/50 bg-primary px-8 py-4 text-sm font-semibold text-white shadow-[0_0_20px_rgba(44,44,226,0.4)] transition-all hover:scale-[1.02] hover:bg-[#3a3af0]"
          >
            Try Playground
          </Link>
          <Link
            href="/docs"
            className="rounded-xl border border-white/20 bg-black/20 px-8 py-4 text-sm font-semibold text-white transition-all hover:scale-[1.02] hover:border-white/35 hover:bg-black/35"
          >
            View Documentation
          </Link>
        </div>
      </div>

      <div className="relative lg:pt-16">
        <CodePreview />
      </div>
    </section>
  );
}
