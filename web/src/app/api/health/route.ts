import { stat } from "fs/promises";
import { NextResponse } from "next/server";
import { COMPILER_CANDIDATES, resolveCompilerPathCached } from "@/lib/compilerPath";

export const runtime = "nodejs";
export const dynamic = "force-dynamic";
export const maxDuration = 5;

export async function GET() {
  const checkedAt = new Date().toISOString();
  const compilerPath = await resolveCompilerPathCached();

  if (!compilerPath) {
    return NextResponse.json(
      {
        ok: false,
        compilerReady: false,
        checkedAt,
        message: "Compiler binary not found. Build backend/compiler/build/astroscript(.exe).",
        candidates: COMPILER_CANDIDATES,
      },
      { status: 503 },
    );
  }

  const details = await stat(compilerPath);

  return NextResponse.json({
    ok: true,
    compilerReady: true,
    checkedAt,
    compilerPath,
    candidates: COMPILER_CANDIDATES,
    modifiedAt: details.mtime.toISOString(),
    sizeBytes: details.size,
  });
}
