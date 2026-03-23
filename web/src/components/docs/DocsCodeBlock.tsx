"use client";

import { useMemo, useState } from "react";

type DocsCodeBlockProps = {
  filename: string;
  code: string;
  language?: "shell" | "astroscript" | "plaintext";
};

function escapeHtml(input: string): string {
  return input
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

function highlightCode(code: string, language: DocsCodeBlockProps["language"]) {
  let html = escapeHtml(code);

  if (language === "shell") {
    html = html
      .replace(/(curl|astro|cd|sh)\b/g, '<span class="text-[#8ab4ff]">$1</span>')
      .replace(/(https:\/\/[^\s]+)/g, '<span class="text-[#71d8ad]">$1</span>');
  }

  if (language === "astroscript") {
    html = html
      .replace(/(\$\*[\s\S]*?\*\$)/g, '<span class="text-[#6b7396]">$1</span>')
      .replace(/(\$\$.*)$/gm, '<span class="text-[#6b7396]">$1</span>')
      .replace(/("([^"\\]|\\.)*")/g, '<span class="text-[#71d8ad]">$1</span>')
      .replace(/\b(-?\d+(?:\.\d+)?)\b/g, '<span class="text-[#f4c97b]">$1</span>')
      .replace(/(:->|:=|==|!=|<=|>=|\*\*|[+\-*/<>])/g, '<span class="text-[#f8d189]">$1</span>')
      .replace(
        /\b(mission|launch|success|abort|telemetry|limit|verify|else_verify|otherwise|orbit|while|times|scenario|trajectory|fallback|stage_sep|coast|broadcast|receive|alarm|command|back|count|real|precise|flag|symbol|voidspace|module|deploy|extends|public|private|this|fleet|mode|alias|wait|tick|root|flr|ceil|abs|logarithm|sine|cosine|tan|asine|acosine|atan|prime|add|minus|mul|divide|mod|AND|OR|NOT|XOR)\b/g,
        '<span class="text-[#8ab4ff]">$1</span>',
      );
  }

  return html;
}

export default function DocsCodeBlock({ filename, code, language = "plaintext" }: DocsCodeBlockProps) {
  const [copied, setCopied] = useState(false);
  const highlighted = useMemo(() => highlightCode(code, language), [code, language]);

  async function handleCopy() {
    try {
      await navigator.clipboard.writeText(code);
      setCopied(true);
      window.setTimeout(() => setCopied(false), 1200);
    } catch {
      setCopied(false);
    }
  }

  return (
    <figure className="overflow-hidden rounded-xl border border-white/10 bg-[#1a1a2e]" role="group" aria-label={`${filename} code block`}>
      <figcaption className="flex items-center justify-between border-b border-white/10 bg-[#20284a] px-4 py-2.5">
        <span className="text-[11px] font-semibold uppercase tracking-[0.18em] text-white/65">{filename}</span>
        <button
          type="button"
          onClick={handleCopy}
          aria-label={`Copy ${filename} code`}
          className="text-[11px] font-semibold uppercase tracking-[0.12em] text-white/60 transition-colors hover:text-white"
        >
          {copied ? "Copied" : "Copy"}
        </button>
      </figcaption>
      <pre className="overflow-x-auto p-5 font-mono text-[13px] leading-7 text-white/90">
        <code dangerouslySetInnerHTML={{ __html: highlighted }} />
      </pre>
    </figure>
  );
}
