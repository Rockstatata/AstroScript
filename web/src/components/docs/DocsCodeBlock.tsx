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
  // First escape HTML entities
  let safeCode = escapeHtml(code);

  if (language === "shell") {
    // For shell, do simple replacements
    safeCode = safeCode
      .replace(/(curl|astro|cd|sh)\b/g, '<span class="text-[#8ab4ff]">$1</span>')
      .replace(/(https:\/\/[^\s]+)/g, '<span class="text-[#71d8ad]">$1</span>');
  }

  if (language === "astroscript") {
    // Tokenize and highlight - process character by character to avoid matching inside already-added spans
    let result = "";
    let i = 0;

    while (i < safeCode.length) {
      // Check for comments
      if (safeCode.substr(i, 2) === "$*") {
        const endIdx = safeCode.indexOf("*$", i + 2);
        if (endIdx !== -1) {
          result += '<span class="text-[#6b7396]">' + safeCode.substring(i, endIdx + 2) + '</span>';
          i = endIdx + 2;
          continue;
        }
      }

      // Check for line comments
      if (safeCode[i] === "$" && safeCode[i + 1] === "$") {
        let endIdx = safeCode.indexOf("\n", i);
        if (endIdx === -1) endIdx = safeCode.length;
        result += '<span class="text-[#6b7396]">' + safeCode.substring(i, endIdx) + '</span>';
        i = endIdx;
        continue;
      }

      // Check for strings
      if (safeCode[i] === '"') {
        let endIdx = i + 1;
        while (endIdx < safeCode.length) {
          if (safeCode[endIdx] === '"' && safeCode[endIdx - 1] !== "\\") break;
          endIdx++;
        }
        result += '<span class="text-[#71d8ad]">' + safeCode.substring(i, endIdx + 1) + '</span>';
        i = endIdx + 1;
        continue;
      }

      // Check for numbers (including decimals)
      if (/\d/.test(safeCode[i])) {
        let endIdx = i;
        let hasDecimal = false;
        while (endIdx < safeCode.length && (/\d/.test(safeCode[endIdx]) || (safeCode[endIdx] === "." && !hasDecimal))) {
          if (safeCode[endIdx] === ".") hasDecimal = true;
          endIdx++;
        }
        result += '<span class="text-[#f4c97b]">' + safeCode.substring(i, endIdx) + '</span>';
        i = endIdx;
        continue;
      }

      // Check for identifiers and keywords
      if (/[a-zA-Z_]/.test(safeCode[i])) {
        let endIdx = i;
        while (endIdx < safeCode.length && /[a-zA-Z0-9_]/.test(safeCode[endIdx])) {
          endIdx++;
        }
        const word = safeCode.substring(i, endIdx);
        const keywords =
          /^(mission|launch|success|abort|telemetry|limit|verify|else_verify|otherwise|orbit|while|times|scenario|trajectory|fallback|stage_sep|coast|broadcast|receive|alarm|command|back|count|real|precise|flag|symbol|voidspace|module|deploy|extends|public|private|this|fleet|mode|alias|wait|tick|root|flr|ceil|abs|logarithm|sine|cosine|tan|asine|acosine|atan|prime|add|minus|mul|divide|mod|AND|OR|NOT|XOR)$/;

        if (keywords.test(word)) {
          result += '<span class="text-[#8ab4ff]">' + word + '</span>';
        } else {
          result += word;
        }
        i = endIdx;
        continue;
      }

      // Check for operators
      if (safeCode.substr(i, 3) === ":->") {
        result += '<span class="text-[#f8d189]">:-></span>';
        i += 3;
        continue;
      }
      if (safeCode.substr(i, 2) === ":=" || safeCode.substr(i, 2) === "==" || safeCode.substr(i, 2) === "!=" || safeCode.substr(i, 2) === "<=" || safeCode.substr(i, 2) === ">=" || safeCode.substr(i, 2) === "**") {
        result += '<span class="text-[#f8d189]">' + safeCode.substring(i, i + 2) + '</span>';
        i += 2;
        continue;
      }

      if (/[+\-*/<>]/.test(safeCode[i])) {
        result += '<span class="text-[#f8d189]">' + safeCode[i] + '</span>';
        i++;
        continue;
      }

      // Default: just add the character as-is
      result += safeCode[i];
      i++;
    }

    return result;
  }

  return safeCode;
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
