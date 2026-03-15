import { type ReactNode } from "react";

type CodeLine = {
  number: number;
  content: ReactNode;
};

type CodeEditorPanelProps = {
  lines: CodeLine[];
};

export function CodeEditorPanel({ lines }: CodeEditorPanelProps) {
  return (
    <section className="flex h-full min-h-[360px] flex-col bg-[#090b1b]" aria-label="Code editor panel">
      <div className="flex h-10 items-center gap-2 border-b border-white/10 px-4 text-xs uppercase tracking-[0.08em] text-[#8a91b6]">
        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" aria-hidden="true">
          <path d="M8 3h8l5 5v13H3V3h5Z" stroke="currentColor" strokeWidth="1.6" strokeLinejoin="round" />
          <path d="M16 3v5h5" stroke="currentColor" strokeWidth="1.6" strokeLinejoin="round" />
        </svg>
        <span>main.astro</span>
      </div>

      <div
        className="flex-1 overflow-auto p-4 font-mono text-[15px] leading-7 text-[#dce0f6]"
        contentEditable
        suppressContentEditableWarning
        spellCheck={false}
        aria-label="AstroScript code editor"
        role="textbox"
      >
        {lines.map((line) => (
          <div key={line.number} className="group flex min-h-7">
            <span className="w-9 select-none pr-3 text-right text-[#4f567a]">{line.number}</span>
            <div className="whitespace-pre">{line.content}</div>
          </div>
        ))}
      </div>
    </section>
  );
}
