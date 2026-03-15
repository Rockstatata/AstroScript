type PlaygroundTab = "output" | "tokens" | "ir" | "errors";

type OutputPanelProps = {
  activeTab: PlaygroundTab;
  onChangeTab: (tab: PlaygroundTab) => void;
  running: boolean;
  output: string;
  tokens: string;
  ir: string;
  error: string;
  stdout: string;
  stderr: string;
};

const tabs: { key: PlaygroundTab; label: string }[] = [
  { key: "output", label: "Output" },
  { key: "tokens", label: "Tokens" },
  { key: "ir", label: "Intermediate Code" },
  { key: "errors", label: "Errors" },
];

function getTabText(props: OutputPanelProps): string {
  if (props.running) {
    return "Compiling mission...";
  }

  if (props.activeTab === "output") {
    return props.output || "Run your AstroScript mission to view runtime output.";
  }

  if (props.activeTab === "tokens") {
    return (
      props.tokens ||
      "Token stream is not emitted by this compiler build. Check the raw compiler stream:\n\n" +
        (props.stdout || "No stdout available.")
    );
  }

  if (props.activeTab === "ir") {
    return props.ir || "No intermediate code was generated.";
  }

  return props.error || props.stderr || "No errors.";
}

export function OutputPanel(props: OutputPanelProps) {
  const bodyText = getTabText(props);

  return (
    <section className="flex h-full min-h-80 flex-col border-t border-white/10 bg-[#0b0d1f] md:border-t-0 md:border-l md:border-white/10" aria-label="Compiler output panel">
      <div className="flex h-10 items-center gap-1 border-b border-white/10 px-3 text-[11px] uppercase tracking-[0.08em] text-[#7f86ac] md:px-4">
        {tabs.map((tab) => (
          <button
            key={tab.key}
            type="button"
            onClick={() => props.onChangeTab(tab.key)}
            aria-label={`Open ${tab.label} tab`}
            className={`h-full border-b px-2 transition-colors md:px-3 ${
              props.activeTab === tab.key
                ? "border-[#2f3ce8] text-[#9aa5ff]"
                : "border-transparent hover:text-[#cad0ee]"
            }`}
          >
            {tab.label}
          </button>
        ))}
      </div>

      <div className="flex-1 overflow-auto px-4 py-3 font-mono text-sm leading-7 text-[#d0d5ef]">
        <pre className="whitespace-pre-wrap wrap-break-word">{bodyText}</pre>

        <div className="mt-5 rounded-md border border-[#28398b]/60 bg-[#131a3b]/70 px-3 py-3 text-xs leading-6 text-[#b8c0ef]">
          <p>
            <span className="mr-2 inline-flex h-4 w-4 items-center justify-center rounded-full bg-[#2435a9] text-[10px] font-semibold text-[#d7defe]">
              i
            </span>
            {props.running
              ? "Compiler process is running. Results will appear here as soon as execution completes."
              : "Use the tabs to inspect runtime output, token stream, intermediate code, and parser errors."}
          </p>
        </div>
      </div>
    </section>
  );
}
