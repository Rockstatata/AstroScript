import { mkdir, readFile, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const repoRoot = resolve(__dirname, "..", "..");
const manifestPath = resolve(repoRoot, "examples", "modules", "index.json");
const outputPath = resolve(__dirname, "..", "src", "lib", "playground-examples.generated.ts");

function asNormalizedCode(content) {
  const normalized = content.replace(/\r\n/g, "\n").trim();
  return `${normalized}\n`;
}

async function generatePlaygroundExamples() {
  const manifestText = await readFile(manifestPath, "utf8");
  const manifest = JSON.parse(manifestText);

  if (!Array.isArray(manifest)) {
    throw new Error("examples/modules/index.json must be an array");
  }

  const seenKeys = new Set();
  const output = {};

  for (const entry of manifest) {
    if (!entry || typeof entry !== "object") {
      throw new Error("Each manifest entry must be an object");
    }

    const { key, label, file, description } = entry;

    if (!key || !label || !file) {
      throw new Error("Each manifest entry requires key, label, and file fields");
    }

    if (seenKeys.has(key)) {
      throw new Error(`Duplicate playground example key: ${key}`);
    }

    seenKeys.add(key);
    const sourcePath = resolve(repoRoot, "examples", "modules", file);
    const sourceText = await readFile(sourcePath, "utf8");

    output[key] = {
      label,
      description: description ?? "",
      sourceFile: `examples/modules/${file}`,
      code: asNormalizedCode(sourceText),
    };
  }

  const generated = `// AUTO-GENERATED FILE. DO NOT EDIT.\n// Source: examples/modules/index.json\n\nexport const playgroundExamples = ${JSON.stringify(output, null, 2)} as const;\n`;

  await mkdir(dirname(outputPath), { recursive: true });
  await writeFile(outputPath, generated, "utf8");

  console.log(`Generated ${outputPath}`);
}

generatePlaygroundExamples().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
