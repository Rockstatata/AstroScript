export function StatusBar() {
  return (
    <footer className="relative z-10 flex h-8 items-center justify-between border-t border-white/10 bg-[#0b0d1f] px-3 text-[11px] uppercase tracking-[0.06em] text-[#7e86a7] md:px-4">
      <div className="flex items-center gap-3 font-mono">
        <span className="inline-flex items-center gap-1">
          <span className="h-2 w-2 rounded-full bg-[#2adf78]" />
          Engine Ready
        </span>
        <span>4.2 MB</span>
      </div>
      <div className="flex items-center gap-4 font-mono">
        <span>Ln 12, Col 24</span>
        <span>UTF-8</span>
      </div>
    </footer>
  );
}
