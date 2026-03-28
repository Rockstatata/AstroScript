import { stat } from "fs/promises";
import { NextResponse } from "next/server";
import { getCompilerCandidates, resolveCompilerPathCached } from "@/lib/compilerPath";

export const runtime = "nodejs";
export const dynamic = "force-dynamic";
export const maxDuration = 5;

export async function GET() {
  const checkedAt = new Date().toISOString();
  const compilerTarget = process.env.ASTROSCRIPT_COMPILER_TARGET ?? "auto";
  const compilerCandidates = getCompilerCandidates();
  const compilerPath = await resolveCompilerPathCached();

  if (!compilerPath) {
    return NextResponse.json(
      {
        ok: false,
        compilerReady: false,
        checkedAt,
        compilerTarget,
        message:
          "Compiler binary not found. Place binaries in web/src/server/compiler or set ASTROSCRIPT_COMPILER_PATH. Toggle preference with ASTROSCRIPT_COMPILER_TARGET=linux|windows.",
        candidates: compilerCandidates,
      },
      { status: 503 },
    );
  }

  const details = await stat(compilerPath);

  return NextResponse.json({
    ok: true,
    compilerReady: true,
    checkedAt,
    compilerTarget,
    compilerPath,
    candidates: compilerCandidates,
    modifiedAt: details.mtime.toISOString(),
    sizeBytes: details.size,
  });
}
