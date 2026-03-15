type StatusBarProps = {
  running: boolean;
  line: number;
  column: number;
  lastRunMs: number | null;
};

export function StatusBar({ running, line, column, lastRunMs }: StatusBarProps) {
  return (
    <footer className="relative z-10 flex h-8 items-center justify-between border-t border-white/10 bg-[#0b0d1f] px-3 text-[11px] uppercase tracking-[0.06em] text-[#7e86a7] md:px-4">
      <div className="flex items-center gap-3 font-mono">
        <span className="inline-flex items-center gap-1">
          <span className={`h-2 w-2 rounded-full ${running ? "bg-[#f4c97b]" : "bg-[#2adf78]"}`} />
          {running ? "Running" : "Engine Ready"}
        </span>
        <span>{lastRunMs === null ? "No recent run" : `${lastRunMs} ms`}</span>
      </div>
      <div className="flex items-center gap-4 font-mono">
        <span>{`Ln ${line}, Col ${column}`}</span>
        <span>UTF-8</span>
      </div>
    </footer>
  );
}
