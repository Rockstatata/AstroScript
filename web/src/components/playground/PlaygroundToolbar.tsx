type PlaygroundToolbarProps = {
  onRun: () => void;
  onClear: () => void;
  running: boolean;
  selectedExample: string;
  examples: Array<{ key: string; label: string }>;
  onSelectExample: (key: string) => void;
};

export function PlaygroundToolbar({
  onRun,
  onClear,
  running,
  selectedExample,
  examples,
  onSelectExample,
}: PlaygroundToolbarProps) {
  return (
    <div className="relative z-10 flex h-12 items-center justify-between border-b border-white/10 bg-[#101124]/95 px-3 text-sm md:px-4">
      <div className="flex items-center gap-2 md:gap-3">
        <button
          type="button"
          onClick={onRun}
          aria-label="Run code"
          disabled={running}
          className="inline-flex items-center gap-2 rounded-md bg-[#2c2ce2] px-4 py-1.5 font-semibold text-white shadow-[0_0_15px_rgba(44,44,226,0.4)] transition-colors hover:bg-[#3a3ae7] disabled:cursor-not-allowed disabled:opacity-70"
        >
          <svg width="12" height="12" viewBox="0 0 12 12" fill="none" aria-hidden="true">
            <path d="M3 2.5 9 6 3 9.5v-7Z" fill="currentColor" />
          </svg>
          {running ? "Running..." : "Run"}
        </button>

        <button
          type="button"
          onClick={onClear}
          aria-label="Clear editor"
          className="inline-flex items-center gap-2 rounded-md px-3 py-1.5 text-[#bcc0db] transition-colors hover:bg-white/5 hover:text-white"
        >
          <svg width="12" height="12" viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path d="M4 6h16M10 11v6m4-6v6M8 6l1-2h6l1 2m-9 0 1 14h8l1-14" stroke="currentColor" strokeWidth="1.6" strokeLinecap="round" />
          </svg>
          Clear
        </button>

        <label className="inline-flex items-center gap-2 rounded-md px-3 py-1.5 text-[#bcc0db] transition-colors hover:bg-white/5 hover:text-white">
          <svg width="12" height="12" viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <path d="M4 6h16M4 12h12M4 18h8" stroke="currentColor" strokeWidth="1.6" strokeLinecap="round" />
          </svg>
          <span className="hidden lg:inline">Example Program</span>
          <select
            aria-label="Select example program"
            value={selectedExample}
            onChange={(event) => onSelectExample(event.target.value)}
            className="rounded border border-white/15 bg-[#12142e] px-2 py-1 text-xs text-white outline-none"
          >
            {examples.map((example) => (
              <option key={example.key} value={example.key}>
                {example.label}
              </option>
            ))}
          </select>
        </label>
      </div>

      <div className="hidden items-center gap-2 md:flex">
        <button
          type="button"
          aria-label="Target selector"
          className="rounded-md border border-white/15 bg-[#131531] px-3 py-1.5 text-xs text-[#a8adcb]"
        >
          # Target: WASM v2
        </button>
        <button
          type="button"
          aria-label="Share"
          className="flex h-8 w-8 items-center justify-center rounded-md text-[#a8adcb] transition-colors hover:bg-white/5 hover:text-white"
        >
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" aria-hidden="true">
            <circle cx="18" cy="5" r="2" stroke="currentColor" strokeWidth="1.6" />
            <circle cx="6" cy="12" r="2" stroke="currentColor" strokeWidth="1.6" />
            <circle cx="18" cy="19" r="2" stroke="currentColor" strokeWidth="1.6" />
            <path d="m8 12 8-6M8 12l8 6" stroke="currentColor" strokeWidth="1.6" strokeLinecap="round" />
          </svg>
        </button>
      </div>
    </div>
  );
}
