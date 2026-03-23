import { NextRequest, NextResponse } from "next/server";
import { execFile } from "child_process";
import { mkdir, unlink, writeFile } from "fs/promises";
import { join } from "path";
import { randomUUID } from "crypto";
import { tmpdir } from "os";
import { COMPILER_ERROR_PATTERN, resolveCompilerPathCached } from "@/lib/compilerPath";

export const runtime = "nodejs";
export const dynamic = "force-dynamic";
export const maxDuration = 30;

type DiagnosticKind = "lexical" | "syntax" | "semantic" | "runtime" | "unknown";

type CompilerDiagnostic = {
  kind: DiagnosticKind;
  message: string;
  line?: number;
  raw: string;
  humanMessage?: string;
  fixHint?: string;
};

function getLineText(code: string, line?: number): string {
  if (!line || line < 1) {
    return "";
  }

  const lines = code.split(/\r?\n/);
  return lines[line - 1] ?? "";
}

function looksLikeMissingTerminator(lineText: string): boolean {
  const trimmed = lineText.trim();
  if (!trimmed) {
    return false;
  }

  if (trimmed.endsWith(".") || trimmed.endsWith("{") || trimmed.endsWith("}")) {
    return false;
  }

  return /(telemetry|limit|transmit|receive|back|wait|stage_sep|coast|:=|\b(add|minus|mul|divide|mod)\b)/.test(trimmed);
}

function enrichDiagnostics(diagnostics: CompilerDiagnostic[], code: string): CompilerDiagnostic[] {
  return diagnostics.map((diagnostic) => {
    const lineText = getLineText(code, diagnostic.line);

    if (diagnostic.kind === "syntax") {
      if (looksLikeMissingTerminator(lineText)) {
        return {
          ...diagnostic,
          humanMessage: `Syntax issue near line ${diagnostic.line ?? "?"}: this statement likely needs a period (.) terminator.`,
          fixHint: "Add a trailing '.' at the end of the statement. AstroScript statements are period-terminated.",
        };
      }

      return {
        ...diagnostic,
        humanMessage: `Syntax issue near line ${diagnostic.line ?? "?"}: the parser could not continue from this token sequence.`,
        fixHint: "Check bracket/parenthesis pairing and statement terminators (.).",
      };
    }

    if (diagnostic.kind === "semantic") {
      if (/variable\s+\w+\s+not declared/i.test(diagnostic.message)) {
        return {
          ...diagnostic,
          humanMessage: `Semantic issue on line ${diagnostic.line ?? "?"}: you are using a variable before declaration.`,
          fixHint: "Declare it first with telemetry, for example: telemetry count x.",
        };
      }

      if (/array\s+\w+\s+not declared/i.test(diagnostic.message)) {
        return {
          ...diagnostic,
          humanMessage: `Semantic issue on line ${diagnostic.line ?? "?"}: array usage found before declaration.`,
          fixHint: "Declare the array before use, for example: telemetry count arr[5].",
        };
      }

      if (/already declared/i.test(diagnostic.message)) {
        return {
          ...diagnostic,
          humanMessage: `Semantic issue on line ${diagnostic.line ?? "?"}: duplicate declaration detected.`,
          fixHint: "Rename one declaration or remove the duplicate definition.",
        };
      }

      return {
        ...diagnostic,
        humanMessage: `Semantic issue on line ${diagnostic.line ?? "?"}: ${diagnostic.message}.`,
        fixHint: "Review declarations and type/identifier usage in this scope.",
      };
    }

    if (diagnostic.kind === "lexical") {
      return {
        ...diagnostic,
        humanMessage: `Lexical issue on line ${diagnostic.line ?? "?"}: unknown or invalid character sequence encountered.`,
        fixHint: "Use valid AstroScript tokens and remove unsupported characters.",
      };
    }

    return {
      ...diagnostic,
      humanMessage: diagnostic.message,
      fixHint: "Review the highlighted line and adjust the statement structure.",
    };
  });
}

function buildUserMessage(diagnostics: CompilerDiagnostic[]): string {
  if (diagnostics.length === 0) {
    return "Compilation failed. Please review the diagnostics below.";
  }

  const first = diagnostics[0];
  const lineText = first.line ? ` on line ${first.line}` : "";
  const primary = `${first.humanMessage ?? `Compilation issue${lineText}: ${first.message}`}`;
  const hint = first.fixHint ? ` Suggested fix: ${first.fixHint}` : "";
  return `${primary}${hint}`;
}

function parseCompilerDiagnosticLine(line: string): CompilerDiagnostic | null {
  const trimmed = line.trim();
  if (!trimmed || !COMPILER_ERROR_PATTERN.test(trimmed)) {
    return null;
  }

  const lexical = trimmed.match(/^LEXICAL ERROR at line (\d+):\s*(.+)$/i);
  if (lexical) {
    return {
      kind: "lexical",
      line: Number.parseInt(lexical[1], 10),
      message: lexical[2],
      raw: trimmed,
    };
  }

  const syntax = trimmed.match(/^SYNTAX ERROR at line (\d+):\s*(.+)$/i);
  if (syntax) {
    return {
      kind: "syntax",
      line: Number.parseInt(syntax[1], 10),
      message: syntax[2],
      raw: trimmed,
    };
  }

  const semantic = trimmed.match(/^Semantic Error(?: at line (\d+))?:\s*(.+)$/i);
  if (semantic) {
    return {
      kind: "semantic",
      line: semantic[1] ? Number.parseInt(semantic[1], 10) : undefined,
      message: semantic[2],
      raw: trimmed,
    };
  }

  const runtime = trimmed.match(/^RUNTIME ERROR(?: at line (\d+))?:\s*(.+)$/i);
  if (runtime) {
    return {
      kind: "runtime",
      line: runtime[1] ? Number.parseInt(runtime[1], 10) : undefined,
      message: runtime[2],
      raw: trimmed,
    };
  }

  return {
    kind: "unknown",
    message: trimmed,
    raw: trimmed,
  };
}

function parseCompilerDiagnostics(outputText: string): CompilerDiagnostic[] {
  const diagnostics: CompilerDiagnostic[] = [];
  const seen = new Set<string>();

  for (const line of outputText.split("\n")) {
    const parsed = parseCompilerDiagnosticLine(line);
    if (!parsed) {
      continue;
    }

    const key = `${parsed.kind}|${parsed.line ?? 0}|${parsed.message}`;
    if (seen.has(key)) {
      continue;
    }

    seen.add(key);
    diagnostics.push(parsed);
  }

  return diagnostics;
}

export async function POST(request: NextRequest) {
  let tempFile = "";

  try {
    const { code } = await request.json();

    if (!code || typeof code !== "string") {
      return NextResponse.json({ error: "No code provided" }, { status: 400 });
    }

    const compilerPath = await resolveCompilerPathCached();
    if (!compilerPath) {
      return NextResponse.json(
        {
          error:
            "AstroScript compiler binary is missing. Build backend/compiler/build/astroscript(.exe) first so the playground can execute code.",
        },
        { status: 500 },
      );
    }

    const tempDir = join(tmpdir(), "astroscript");
    await mkdir(tempDir, { recursive: true });

    tempFile = join(tempDir, `${randomUUID()}.as`);
    await writeFile(tempFile, code, "utf-8");

    const result = await new Promise<{ stdout: string; stderr: string }>(
      (resolve, reject) => {
        execFile(
          compilerPath,
          [tempFile],
          { timeout: 10000, maxBuffer: 1024 * 1024 },
          (error, stdout, stderr) => {
            if (error && !stdout && !stderr) {
              reject(error);
              return;
            }
            resolve({ stdout: stdout ?? "", stderr: stderr ?? "" });
          }
        );
      }
    );

    const stdoutText = result.stdout.trim();
    const stderrText = result.stderr.trim();
    const lines = result.stdout.split("\n");
    const irStart = lines.findIndex((l) => l.includes("--- Optimized Three Address Code ---"));
    const irEnd = irStart === -1
      ? -1
      : lines.findIndex((l, i) => i > irStart && l.includes("-----------------------------"));

    let output = "";
    let ir = "";
    let tokens = "";
    let error = "";

    if (irStart !== -1) {
      const irSliceEnd = irEnd !== -1 ? irEnd : lines.length;
      ir = lines.slice(irStart, irSliceEnd).join("\n").trim();
    }

    const execLines = lines.filter((l) => l.startsWith("PRINT: "));
    output = execLines.map((l) => l.replace("PRINT: ", "")).join("\n");

    const tokenLines = lines.filter((l) => /\bTOKEN\b|\bLEXICAL\b|\bPARSER\b/i.test(l));
    tokens = tokenLines.join("\n");

    const parsedDiagnostics = parseCompilerDiagnostics([stdoutText, stderrText].filter(Boolean).join("\n"));
    const diagnostics = enrichDiagnostics(parsedDiagnostics, code);
    const diagnosticText = diagnostics
      .map((entry) => (entry.line ? `line ${entry.line}: ${entry.message}` : entry.message))
      .join("\n")
      .trim();

    if (diagnostics.length > 0 || stderrText) {
      error = diagnosticText || stderrText;
      const userMessage = buildUserMessage(diagnostics);
      return NextResponse.json({
        output,
        tokens,
        ir,
        stdout: stdoutText,
        stderr: stderrText,
        error,
        userMessage,
        diagnostics,
      }, { status: 422 });
    }

    return NextResponse.json({ output, tokens, ir, stdout: stdoutText, stderr: stderrText, diagnostics: [] });
  } catch (err) {
    const message = err instanceof Error ? err.message : "Compilation failed";
    return NextResponse.json({ error: message }, { status: 500 });
  } finally {
    if (tempFile) {
      unlink(tempFile).catch(() => {});
    }
  }
}
