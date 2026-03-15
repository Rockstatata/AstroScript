import { NextRequest, NextResponse } from "next/server";
import { exec } from "child_process";
import { writeFile, unlink, mkdir } from "fs/promises";
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

    // Parse output: separate execution output from IR code
    const lines = result.stdout.split("\n");
    const irStart = lines.findIndex((l) => l.includes("--- Optimized Three Address Code ---"));
    const irEnd = lines.findIndex((l, i) => i > irStart && l.includes("-----------------------------"));
    const symbolStart = lines.findIndex((l) => l.includes("--- Symbol Table ---"));

    let output = "";
    let ir = "";

    if (irStart !== -1 && irEnd !== -1) {
      ir = lines.slice(irStart, irEnd + 1).join("\n");
    }

    // Execution output is the PRINT lines between IR end and symbol table
    const execLines = lines.filter((l) => l.startsWith("PRINT: "));
    output = execLines.map((l) => l.replace("PRINT: ", "")).join("\n");

    if (result.stderr) {
      return NextResponse.json({
        output,
        ir,
        error: result.stderr,
      });
    }

    return NextResponse.json({ output, ir });
  } catch (err) {
    const message = err instanceof Error ? err.message : "Compilation failed";
    return NextResponse.json({ error: message }, { status: 500 });
  } finally {
    if (tempFile) {
      unlink(tempFile).catch(() => {});
    }
  }
}
