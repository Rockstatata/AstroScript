import { NextRequest, NextResponse } from "next/server";
import { exec } from "child_process";
import { access, mkdir, unlink, writeFile } from "fs/promises";
import { join } from "path";
import { randomUUID } from "crypto";
import { tmpdir } from "os";

const COMPILER_PATH = join(process.cwd(), "..", "backend", "compiler", "build", "astroscript");

export async function POST(request: NextRequest) {
  let tempFile = "";

  try {
    const { code } = await request.json();

    if (!code || typeof code !== "string") {
      return NextResponse.json({ error: "No code provided" }, { status: 400 });
    }

    try {
      await access(COMPILER_PATH);
    } catch {
      return NextResponse.json(
        {
          error:
            "AstroScript compiler binary is missing. Build backend/compiler first so the playground can execute code.",
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
        exec(
          `"${COMPILER_PATH}" "${tempFile}"`,
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

    const lines = result.stdout.split("\n");
    const irStart = lines.findIndex((l) => l.includes("--- Optimized Three Address Code ---"));
    const irEnd = lines.findIndex(
      (l, i) => i > irStart && i !== irStart && l.startsWith("---") && l.trim() !== "--- Optimized Three Address Code ---",
    );

    let output = "";
    let ir = "";
    let tokens = "";

    if (irStart !== -1) {
      const irSliceEnd = irEnd !== -1 ? irEnd : lines.length;
      ir = lines.slice(irStart, irSliceEnd).join("\n").trim();
    }

    const execLines = lines.filter((l) => l.startsWith("PRINT: "));
    output = execLines.map((l) => l.replace("PRINT: ", "")).join("\n");

    const tokenLines = lines.filter((l) => /\bTOKEN\b|\bLEXICAL\b|\bPARSER\b/i.test(l));
    tokens = tokenLines.join("\n");

    const stderrText = result.stderr.trim();
    const stdoutText = result.stdout.trim();

    if (result.stderr) {
      return NextResponse.json({
        output,
        tokens,
        ir,
        stdout: stdoutText,
        stderr: stderrText,
        error: stderrText,
      });
    }

    return NextResponse.json({ output, tokens, ir, stdout: stdoutText, stderr: stderrText });
  } catch (err) {
    const message = err instanceof Error ? err.message : "Compilation failed";
    return NextResponse.json({ error: message }, { status: 500 });
  } finally {
    if (tempFile) {
      unlink(tempFile).catch(() => {});
    }
  }
}
