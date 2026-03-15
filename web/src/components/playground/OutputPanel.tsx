type PlaygroundTab = "output" | "tokens" | "ir" | "errors";

type OutputPanelProps = {
  activeTab: PlaygroundTab;
  onChangeTab: (tab: PlaygroundTab) => void;
};

const tabs: { key: PlaygroundTab; label: string }[] = [
  { key: "output", label: "Output" },
  { key: "tokens", label: "Tokens" },
  { key: "ir", label: "Intermediate Code" },
  { key: "errors", label: "Errors" },
];

export function OutputPanel({ activeTab, onChangeTab }: OutputPanelProps) {
  return (
    <section className="flex h-full min-h-80 flex-col border-t border-white/10 bg-[#0b0d1f] md:w-112.5 md:border-t-0 md:border-l md:border-white/10" aria-label="Compiler output panel">
      <div className="flex h-10 items-center gap-1 border-b border-white/10 px-3 text-[11px] uppercase tracking-[0.08em] text-[#7f86ac] md:px-4">
        {tabs.map((tab) => (
          <button
            key={tab.key}
            type="button"
            onClick={() => onChangeTab(tab.key)}
            aria-label={`Open ${tab.label} tab`}
            className={`h-full border-b px-2 transition-colors md:px-3 ${
              activeTab === tab.key
                ? "border-[#2f3ce8] text-[#9aa5ff]"
                : "border-transparent hover:text-[#cad0ee]"
            }`}
          >
            {tab.label}
          </button>
        ))}
      </div>

      <div className="flex-1 overflow-auto px-4 py-3 font-mono text-sm leading-7 text-[#d0d5ef]">
        <p>
          <span className="text-[#7180ad]">[10:42:01]</span>{" "}
          <span className="text-[#8de698]">Compiling mission &apos;Apollo11&apos;...</span>
        </p>
        <p>
          <span className="text-[#7180ad]">[10:42:02]</span>{" "}
          <span className="text-[#8de698]">Optimization phase complete.</span>
        </p>
        <p>
          <span className="text-[#7180ad]">[10:42:02]</span> Running sequence...
        </p>
        <p className="mt-2 text-[#b7c1ef]">&gt; Ignition start...</p>
        <p className="text-[#b7c1ef]">&gt; Engine status: ACTIVE (100.0)</p>

        <div className="mt-5 rounded-md border border-[#28398b]/60 bg-[#131a3b]/70 px-3 py-3 text-xs leading-6 text-[#b8c0ef]">
          <p>
            <span className="mr-2 inline-flex h-4 w-4 items-center justify-center rounded-full bg-[#2435a9] text-[10px] font-semibold text-[#d7defe]">
              i
            </span>
            Mission completed in <span className="font-semibold text-white">124ms</span>. Peak memory usage
            <span className="font-semibold text-white"> 4.2MB</span>. Target lunar coordinates locked.
          </p>
        </div>
      </div>
    </section>
  );
}
