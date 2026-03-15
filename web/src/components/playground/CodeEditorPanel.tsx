"use client";

import Editor, { type Monaco, type OnChange } from "@monaco-editor/react";

type CodeEditorPanelProps = {
  code: string;
  onChange: (value: string) => void;
  onCursorChange?: (line: number, column: number) => void;
};

export function CodeEditorPanel({ code, onChange, onCursorChange }: CodeEditorPanelProps) {
  const handleEditorChange: OnChange = (value) => {
    onChange(value ?? "");
  };

  function handleMount(editor: Parameters<NonNullable<React.ComponentProps<typeof Editor>["onMount"]>>[0], monaco: Monaco) {
    editor.onDidChangeCursorPosition((event) => {
      onCursorChange?.(event.position.lineNumber, event.position.column);
    });

    monaco.editor.setTheme("vs-dark");
  }

  return (
    <section className="flex h-full min-h-90 flex-col bg-[#090b1b]" aria-label="Code editor panel">
      <div className="flex h-10 items-center gap-2 border-b border-white/10 px-4 text-xs uppercase tracking-[0.08em] text-[#8a91b6]">
        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" aria-hidden="true">
          <path d="M8 3h8l5 5v13H3V3h5Z" stroke="currentColor" strokeWidth="1.6" strokeLinejoin="round" />
          <path d="M16 3v5h5" stroke="currentColor" strokeWidth="1.6" strokeLinejoin="round" />
        </svg>
        <span>main.astro</span>
      </div>

      <div className="min-h-0 flex-1">
        <Editor
          height="100%"
          defaultLanguage="plaintext"
          value={code}
          onChange={handleEditorChange}
          onMount={handleMount}
          options={{
            minimap: { enabled: false },
            fontSize: 14,
            fontFamily: "var(--font-jetbrains-mono)",
            lineHeight: 24,
            roundedSelection: false,
            scrollBeyondLastLine: false,
            padding: { top: 14, bottom: 14 },
            automaticLayout: true,
            wordWrap: "on",
          }}
        />
      </div>
    </section>
  );
}
