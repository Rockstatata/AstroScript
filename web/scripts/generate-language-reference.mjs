import { mkdir, readFile, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const repoRoot = resolve(__dirname, "..", "..");
const lexerPath = resolve(repoRoot, "backend", "compiler", "lexer", "lexer.l");
const parserPath = resolve(repoRoot, "backend", "compiler", "parser", "parser.y");
const outputPath = resolve(__dirname, "..", "src", "lib", "language-reference.generated.json");

const sectionForKeyword = {
  mission: "Program Structure",
  launch: "Program Structure",
  success: "Program Structure",
  abort: "Program Structure",

  telemetry: "Declarations and Types",
  limit: "Declarations and Types",
  alias: "Declarations and Types",
  fleet: "Declarations and Types",
  mode: "Declarations and Types",

  count: "Types",
  real: "Types",
  precise: "Types",
  flag: "Types",
  symbol: "Types",
  voidspace: "Types",

  verify: "Control Flow",
  else_verify: "Control Flow",
  otherwise: "Control Flow",
  orbit: "Control Flow",
  while: "Control Flow",
  times: "Control Flow",
  scenario: "Control Flow",
  trajectory: "Control Flow",
  fallback: "Control Flow",
  stage_sep: "Control Flow",
  coast: "Control Flow",

  command: "Functions and I/O",
  back: "Functions and I/O",
  transmit: "Functions and I/O",
  receive: "Functions and I/O",
  broadcast: "Functions and I/O",
  alarm: "Functions and I/O",

  module: "Modules",
  deploy: "Modules",
  extends: "Modules",
  public: "Modules",
  private: "Modules",
  this: "Modules",

  wait: "Built-ins and Runtime",
  tick: "Built-ins and Runtime",
  root: "Built-ins and Runtime",
  flr: "Built-ins and Runtime",
  ceil: "Built-ins and Runtime",
  abs: "Built-ins and Runtime",
  logarithm: "Built-ins and Runtime",
  sine: "Built-ins and Runtime",
  cosine: "Built-ins and Runtime",
  tan: "Built-ins and Runtime",
  asine: "Built-ins and Runtime",
  acosine: "Built-ins and Runtime",
  atan: "Built-ins and Runtime",
  prime: "Built-ins and Runtime",

  add: "Operators",
  minus: "Operators",
  mul: "Operators",
  divide: "Operators",
  mod: "Operators",
  AND: "Operators",
  OR: "Operators",
  NOT: "Operators",
  XOR: "Operators",
};

const keywordMeta = {
  mission: { purpose: "Program entry point", equivalent: "main()" },
  launch: { purpose: "Starts mission block", equivalent: "{" },
  success: { purpose: "Successful completion marker", equivalent: "return 0" },
  abort: { purpose: "Abort current mission", equivalent: "exit/fail" },

  telemetry: { purpose: "Variable declaration", equivalent: "declaration" },
  limit: { purpose: "Constant-like declaration", equivalent: "const" },
  alias: { purpose: "Reserved declaration token", equivalent: "reserved" },
  fleet: { purpose: "Reserved declaration token", equivalent: "reserved" },
  mode: { purpose: "Mode declaration", equivalent: "enum-like" },

  count: { purpose: "Integer type", equivalent: "int" },
  real: { purpose: "Single precision number", equivalent: "float" },
  precise: { purpose: "Double precision number", equivalent: "double" },
  flag: { purpose: "Boolean type", equivalent: "bool" },
  symbol: { purpose: "String literal type", equivalent: "string" },
  voidspace: { purpose: "Void return type", equivalent: "void" },

  verify: { purpose: "Conditional branch", equivalent: "if" },
  else_verify: { purpose: "Secondary condition", equivalent: "else if" },
  otherwise: { purpose: "Fallback branch", equivalent: "else" },
  orbit: { purpose: "Loop entry", equivalent: "while/for" },
  while: { purpose: "Orbit while form", equivalent: "while" },
  times: { purpose: "Orbit range form", equivalent: "for" },
  scenario: { purpose: "Switch-like branch", equivalent: "switch" },
  trajectory: { purpose: "Scenario case", equivalent: "case" },
  fallback: { purpose: "Default scenario case", equivalent: "default" },
  stage_sep: { purpose: "Reserved control token", equivalent: "reserved" },
  coast: { purpose: "Reserved control token", equivalent: "reserved" },

  command: { purpose: "Function declaration", equivalent: "function" },
  back: { purpose: "Return from function", equivalent: "return" },
  transmit: { purpose: "Print value", equivalent: "print" },
  receive: { purpose: "Read input", equivalent: "input" },
  broadcast: { purpose: "Reserved IO token", equivalent: "reserved" },
  alarm: { purpose: "Reserved IO token", equivalent: "reserved" },

  module: { purpose: "Class-like declaration", equivalent: "class" },
  deploy: { purpose: "Reserved module token", equivalent: "reserved" },
  extends: { purpose: "Inheritance token", equivalent: "extends" },
  public: { purpose: "Access modifier", equivalent: "public" },
  private: { purpose: "Access modifier", equivalent: "private" },
  this: { purpose: "Current instance token", equivalent: "this" },

  wait: { purpose: "Delay instruction", equivalent: "sleep" },
  tick: { purpose: "Delay unit token", equivalent: "tick" },
  root: { purpose: "Math token", equivalent: "sqrt" },
  flr: { purpose: "Math token", equivalent: "floor" },
  ceil: { purpose: "Math token", equivalent: "ceil" },
  abs: { purpose: "Math token", equivalent: "abs" },
  logarithm: { purpose: "Math token", equivalent: "log" },
  sine: { purpose: "Math token", equivalent: "sin" },
  cosine: { purpose: "Math token", equivalent: "cos" },
  tan: { purpose: "Math token", equivalent: "tan" },
  asine: { purpose: "Math token", equivalent: "asin" },
  acosine: { purpose: "Math token", equivalent: "acos" },
  atan: { purpose: "Math token", equivalent: "atan" },
  prime: { purpose: "Math token", equivalent: "prime check" },

  add: { purpose: "Addition operator", equivalent: "+" },
  minus: { purpose: "Subtraction operator", equivalent: "-" },
  mul: { purpose: "Multiplication operator", equivalent: "*" },
  divide: { purpose: "Division operator", equivalent: "/" },
  mod: { purpose: "Modulo operator", equivalent: "%" },
  AND: { purpose: "Logical and", equivalent: "&&" },
  OR: { purpose: "Logical or", equivalent: "||" },
  NOT: { purpose: "Logical not", equivalent: "!" },
  XOR: { purpose: "Logical xor", equivalent: "xor" },
};

const operatorRows = [
  { operation: "Addition", keyword: "add", symbol: "+" },
  { operation: "Subtraction", keyword: "minus", symbol: "-" },
  { operation: "Multiplication", keyword: "mul", symbol: "*" },
  { operation: "Division", keyword: "divide", symbol: "/" },
  { operation: "Modulo", keyword: "mod", symbol: "" },
  { operation: "Power", keyword: "-", symbol: "**" },
];

function normalizeSectionKey(title) {
  return `${title.toLowerCase().replace(/[^a-z0-9]+/g, "-")}-keywords`;
}

async function generateLanguageReference() {
  const [lexerContent, parserContent] = await Promise.all([
    readFile(lexerPath, "utf8"),
    readFile(parserPath, "utf8"),
  ]);

  const grammarStart = parserContent.indexOf("%%");
  const grammarEnd = parserContent.indexOf("%%", grammarStart + 2);
  const grammarContent = grammarStart !== -1 && grammarEnd !== -1
    ? parserContent.slice(grammarStart, grammarEnd)
    : parserContent;

  const keywordRegex = /"([A-Za-z_]+)"\s*\{\s*return\s+([A-Z_]+);\s*\}/g;
  const keywords = [];

  for (const match of lexerContent.matchAll(keywordRegex)) {
    const keyword = match[1];
    const token = match[2];
    const isUsed = new RegExp(`\\b${token}\\b`).test(grammarContent);
    keywords.push({ keyword, token, isUsed });
  }

  const keywordsBySection = new Map();

  for (const entry of keywords) {
    const section = sectionForKeyword[entry.keyword] ?? "Other";
    if (!keywordsBySection.has(section)) {
      keywordsBySection.set(section, []);
    }

    const meta = keywordMeta[entry.keyword] ?? {
      purpose: "Language token",
      equivalent: "-",
    };

    keywordsBySection.get(section).push({
      keyword: entry.keyword,
      purpose: meta.purpose,
      equivalent: meta.equivalent,
      status: entry.isUsed ? "active" : "reserved",
    });
  }

  const preferredOrder = [
    "Program Structure",
    "Declarations and Types",
    "Types",
    "Control Flow",
    "Functions and I/O",
    "Modules",
    "Built-ins and Runtime",
    "Operators",
    "Other",
  ];

  const keywordSections = preferredOrder
    .filter((title) => keywordsBySection.has(title))
    .map((title) => {
      const rows = keywordsBySection
        .get(title)
        .sort((a, b) => a.keyword.localeCompare(b.keyword));
      return {
        title,
        id: normalizeSectionKey(title),
        rows,
      };
    });

  const hasReservedKeywords = keywordSections.some((section) =>
    section.rows.some((row) => row.status === "reserved")
  );

  const payload = {
    generatedAt: new Date().toISOString(),
    keywordSections,
    operators: {
      arithmetic: operatorRows,
      comparisons: ["<", ">", "<=", ">=", "==", "!="],
      logical: ["AND", "OR", "NOT", "XOR"],
    },
    notes: {
      reservedMeaning: hasReservedKeywords
        ? "Reserved tokens are recognized by the lexer but currently have no grammar production in the parser."
        : "All lexer keywords are currently covered by parser productions and are active.",
    },
  };

  await mkdir(dirname(outputPath), { recursive: true });
  await writeFile(outputPath, `${JSON.stringify(payload, null, 2)}\n`, "utf8");
  console.log(`Generated language reference: ${outputPath}`);
}

generateLanguageReference().catch((error) => {
  console.error(error);
  process.exit(1);
});
