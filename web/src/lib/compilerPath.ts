import { constants } from "fs";
import { access } from "fs/promises";
import { join } from "path";

type CompilerTarget = "auto" | "windows" | "linux";
const ASTROSCRIPT_COMPILER_TARGET = "windows";

function normalizeCompilerTarget(rawTarget?: string): CompilerTarget {
  const normalized = rawTarget?.trim().toLowerCase();
  if (normalized === "windows" || normalized === "linux") {
    return normalized;
  }

  return "auto";
}

function getEffectiveCompilerTarget(): Exclude<CompilerTarget, "auto"> {
  const target = normalizeCompilerTarget(ASTROSCRIPT_COMPILER_TARGET);
  if (target !== "auto") {
    return target;
  }

  return process.platform === "win32" ? "windows" : "linux";
}

function getPreferredBinaryNames(): string[] {
  if (getEffectiveCompilerTarget() === "windows") {
    return ["astroscript-windows.exe", "astroscript.exe"];
  }

  return ["astroscript-linux", "astroscript"];
}

function getCandidateDirectories(): string[] {
  return [
    join(process.cwd(), "src", "server", "compiler"),
    join(process.cwd(), "web", "src", "server", "compiler"),
    join(process.cwd(), "backend", "compiler", "build"),
    join(process.cwd(), "..", "backend", "compiler", "build"),
  ];
}

function dedupeCandidates(candidates: string[]): string[] {
  const unique = new Set<string>();
  for (const candidate of candidates) {
    unique.add(candidate);
  }

  return Array.from(unique);
}

function buildCompilerCandidates(): string[] {
  const envPath = process.env.ASTROSCRIPT_COMPILER_PATH;
  const preferredBinaryNames = getPreferredBinaryNames();
  const candidateDirectories = getCandidateDirectories();

  const generatedCandidates = candidateDirectories.flatMap((directory) =>
    preferredBinaryNames.map((binaryName) => join(directory, binaryName)),
  );

  return dedupeCandidates([envPath, ...generatedCandidates].filter((candidate): candidate is string => Boolean(candidate)));
}

export const COMPILER_CANDIDATES = buildCompilerCandidates();

export function getCompilerCandidates(): string[] {
  return buildCompilerCandidates();
}

function getCompilerAccessMode(): number {
  return process.platform === "win32" ? constants.F_OK : constants.X_OK;
}

export const COMPILER_ERROR_PATTERN = /(LEXICAL ERROR|SYNTAX ERROR|Semantic Error|RUNTIME ERROR|^Error:)/im;

let cachedCompilerPath: string | null = null;
let cacheExpiryMs = 0;
let cachedResolverKey = "";
const CACHE_TTL_MS = 60_000;

function getResolverCacheKey(): string {
  return [
    process.cwd(),
    process.env.ASTROSCRIPT_COMPILER_PATH ?? "",
    normalizeCompilerTarget(ASTROSCRIPT_COMPILER_TARGET),
  ].join("|");
}

export async function resolveCompilerPath(): Promise<string | null> {
  const accessMode = getCompilerAccessMode();

  for (const candidate of getCompilerCandidates()) {
    try {
      await access(candidate, accessMode);
      return candidate;
    } catch {
      // Try the next compiler candidate.
    }
  }

  return null;
}

export async function resolveCompilerPathCached(): Promise<string | null> {
  const now = Date.now();
  const cacheKey = getResolverCacheKey();
  if (cacheKey !== cachedResolverKey) {
    cachedCompilerPath = null;
    cacheExpiryMs = 0;
    cachedResolverKey = cacheKey;
  }

  if (cachedCompilerPath && now < cacheExpiryMs) {
    return cachedCompilerPath;
  }

  const compilerPath = await resolveCompilerPath();
  cachedCompilerPath = compilerPath;
  cacheExpiryMs = now + CACHE_TTL_MS;
  return compilerPath;
}