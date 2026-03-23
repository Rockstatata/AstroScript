import { access } from "fs/promises";
import { join } from "path";

export const COMPILER_CANDIDATES = [
  process.env.ASTROSCRIPT_COMPILER_PATH,
  join(process.cwd(), "..", "backend", "compiler", "build", "astroscript.exe"),
  join(process.cwd(), "..", "backend", "compiler", "build", "astroscript"),
].filter((candidate): candidate is string => Boolean(candidate));

export const COMPILER_ERROR_PATTERN = /(LEXICAL ERROR|SYNTAX ERROR|Semantic Error|RUNTIME ERROR|^Error:)/im;

let cachedCompilerPath: string | null = null;
let cacheExpiryMs = 0;
const CACHE_TTL_MS = 60_000;

export async function resolveCompilerPath(): Promise<string | null> {
  for (const candidate of COMPILER_CANDIDATES) {
    try {
      await access(candidate);
      return candidate;
    } catch {
      // Try the next compiler candidate.
    }
  }

  return null;
}

export async function resolveCompilerPathCached(): Promise<string | null> {
  const now = Date.now();
  if (cachedCompilerPath && now < cacheExpiryMs) {
    return cachedCompilerPath;
  }

  const compilerPath = await resolveCompilerPath();
  cachedCompilerPath = compilerPath;
  cacheExpiryMs = now + CACHE_TTL_MS;
  return compilerPath;
}