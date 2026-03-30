"use client";

import { type CSSProperties, useCallback, useEffect, useMemo, useRef, useState } from "react";
import { CodeEditorPanel } from "@/components/playground/CodeEditorPanel";
import { EditorDivider } from "@/components/playground/EditorDivider";
import Navbar from "@/components/Navbar";
import { OutputPanel } from "@/components/playground/OutputPanel";
import { PlaygroundSidebar } from "@/components/playground/PlaygroundSidebar";
import { PlaygroundToolbar } from "@/components/playground/PlaygroundToolbar";
import { StatusBar } from "@/components/playground/StatusBar";
import { playgroundExamples } from "@/lib/playground-examples.generated";

type PlaygroundTab = "output" | "tokens" | "ir" | "c" | "errors";

type PlaygroundDiagnostic = {
  kind: "lexical" | "syntax" | "semantic" | "runtime" | "unknown";
  message: string;
  line?: number;
  raw: string;
  humanMessage?: string;
  fixHint?: string;
};

type CompileResponse = {
  output?: string;
  tokens?: string;
  ir?: string;
  cCode?: string;
  error?: string;
  stdout?: string;
  stderr?: string;
  userMessage?: string;
  diagnostics?: PlaygroundDiagnostic[];
};

type HealthResponse = {
  compilerReady: boolean;
  message?: string;
  compilerPath?: string;
};

function countReceiveStatements(code: string): number {
  const matches = code.match(/\breceive\b/gi);
  return matches ? matches.length : 0;
}

function promptRuntimeInput(existingInput: string, receiveCount: number): string | null {
  if (receiveCount <= 0) {
    return existingInput;
  }

  const defaults = existingInput.split(/\r?\n/);
  const values: string[] = [];
  for (let index = 0; index < receiveCount; index += 1) {
    const defaultValue = defaults[index] ?? "";
    const userValue = window.prompt(`Runtime input ${index + 1}/${receiveCount}:`, defaultValue);
    if (userValue === null) {
      return null;
    }
    values.push(userValue);
  }

  return values.join("\n");
}

const legacyExamples: Record<string, { label: string; code: string }> = {
  basics: {
    label: "Mission Basics",
    code: `mission Apollo launch
{
    telemetry count astronauts := 3.

    verify (astronauts > 0)
    {
      transmit "Mission ready".
    }

    orbit (astronauts > 0)
    {
      transmit astronauts.
        astronauts := astronauts minus 1.
    }
}
success`,
  },
  declarationsTypes: {
  label: "Module 1: Declarations and Types",
  code: `mission DeclarationsTypes launch
{
  alias count IntAlias.
  telemetry count c := 7.
  telemetry real r := 3.5.
  telemetry precise p := 9.25.
  telemetry flag ok := 1.
  telemetry symbol msg := "types".
  telemetry IntAlias custom := 11.
  limit count MAX_LIMIT := 100.

  fleet count dynFleet[4].
  telemetry count arr[3].
  dynFleet[0] := c add 1.
  dynFleet[1] := dynFleet[0] add 2.
  arr[0] := c.
  arr[1] := custom.
  arr[2] := MAX_LIMIT.

  transmit c.
  transmit r.
  transmit p.
  transmit ok.
  transmit msg.
  transmit custom.
  transmit dynFleet[1].
  transmit arr[1].
}
success`,
  },
  operatorsKeywordAndSymbol: {
  label: "Module 2: Operators (Keyword and Symbol)",
  code: `mission OperatorsShowcase launch
{
  telemetry count a := 12.
  telemetry count b := 5.
  telemetry count out.

  out := a add b.
  transmit out.
  out := a + b.
  transmit out.

  out := a minus b.
  transmit out.
  out := a - b.
  transmit out.

  out := a mul b.
  transmit out.
  out := a * b.
  transmit out.

  out := a divide b.
  transmit out.
  out := a / b.
  transmit out.

  out := a mod b.
  transmit out.

  out := a ** 2.
  transmit out.
}
success`,
  },
  comparisonsAndLogic: {
  label: "Module 3: Comparisons and Logic",
  code: `mission ComparisonsLogic launch
{
  telemetry count x := 6.
  telemetry count y := 3.

  verify (x > y) { transmit "GT". }
  verify (x < y) { transmit "LT". } otherwise { transmit "NOT_LT". }
  verify (x >= y) { transmit "GE". }
  verify (x <= y) { transmit "LE". } otherwise { transmit "NOT_LE". }
  verify (x == y) { transmit "EQ". } otherwise { transmit "NOT_EQ". }
  verify (x != y) { transmit "NEQ". }

  verify ((x > 0) AND (y > 0)) { transmit "AND". }
  verify ((x < 0) OR (y > 0)) { transmit "OR". }
  verify ((x > 0) XOR (y < 0)) { transmit "XOR". }
  verify (NOT (x < 0)) { transmit "NOT". }
}
success`,
  },
  controlFlow: {
  label: "Module 4: verify/else_verify/otherwise",
  code: `mission ControlFlow launch
{
  telemetry count fuel := 5.

    verify (fuel > 3)
    {
      transmit "Launch condition satisfied".
    }
  else_verify (fuel == 3)
  {
    transmit "Edge condition".
  }
    otherwise
    {
      transmit "Fuel below launch threshold".
    }
}
success`,
  },
  loopsAndFlowControl: {
  label: "Module 5: orbit/while/times + coast/stage_sep",
  code: `mission LoopsFlowControl launch
{
  telemetry count i := 0.

  orbit (i < 6)
  {
    i := i add 1.
    verify (i == 2)
    {
      coast.
    }
    transmit i.
    verify (i == 4)
    {
      stage_sep.
    }
  }

  orbit while (i < 7)
  {
    transmit i.
    i := i add 1.
  }

  orbit times (i : 0 : 3)
  {
    transmit i.
  }
}
success`,
  },
  scenarioSwitch: {
  label: "Module 6: scenario/trajectory/fallback",
  code: `mission ScenarioSwitch launch
{
  telemetry count code := 2.

  scenario (code)
  {
    trajectory 1:
      transmit "case 1".
      stage_sep.
    trajectory 2:
      transmit "case 2".
      stage_sep.
    fallback:
      transmit "default".
  }
}
success`,
  },
  functionsAndReturns: {
  label: "Module 7: command/back/: and :->",
  code: `mission FunctionsReturns launch
{
  command add_kw(count a, count b) : count
  {
    back a add b.
  }

  command mul_sym(count a, count b) :-> count
  {
    back a * b.
  }

  command ping() : voidspace
  {
    transmit "ping".
    back.
  }

  telemetry count r1 := add_kw(7, 8).
  telemetry count r2 := mul_sym(3, 4).
  transmit r1.
  transmit r2.
  ping().
}
success`,
  },
  ioAndMessaging: {
  label: "Module 8: transmit/broadcast/alarm/receive",
  code: `mission IOMessaging launch
{
  telemetry symbol txt := "hello".
  telemetry count val.

  transmit txt.
  broadcast "broadcast channel".
  alarm "alarm channel".

  receive val.
  transmit val.
}
success`,
  },
  waitAndMathBuiltins: {
  label: "Module 9: wait/tick + Math Built-ins",
  code: `mission WaitMath launch
{
  telemetry count x := 9.
  telemetry count y.

  y := root(x).
  transmit y.
  y := flr(3.9).
  transmit y.
  y := ceil(3.1).
  transmit y.
  y := abs(-22).
  transmit y.
  y := logarithm(1).
  transmit y.
  y := sine(0).
  transmit y.
  y := cosine(0).
  transmit y.
  y := tan(0).
  transmit y.
  y := asine(0).
  transmit y.
  y := acosine(1).
  transmit y.
  y := atan(1).
  transmit y.
  y := prime(17).
  transmit y.

  wait 1 tick.
}
success`,
  },
  modulesDeployThis: {
  label: "Module 10: OOP (extends/override/super/new)",
  code: `mission MissionOOP launch
{
  module BaseModule
  {
    public telemetry count id := 0.

    command BaseModule(count initial) : voidspace
    {
      this.id := initial.
    }

    command base_id() : count
    {
      back this.id.
    }

    command add_task(count incoming) : count
    {
      this.id := this.id add incoming.
      back this.id.
    }
  }

  module ChildModule extends BaseModule
  {
    public telemetry count emergencyBoost := 2.

    command ChildModule(count initial) : voidspace
    {
      this.id := initial.
    }

    override command add_task(count incoming) : count
    {
      back super.add_task(incoming add this.emergencyBoost).
    }
  }

  deploy ChildModule ship(10).
  transmit ship.add_task(3).
  transmit ship.id.

  telemetry ChildModule backup := new ChildModule(20).
  transmit backup.add_task(1).
}
success`,
  },
  overloadingAndScope: {
  label: "Module 11: Command/Method Overloading + Scoped Variables",
  code: `mission OverloadingScope launch
{
  command route(count normal) : count
  {
    back normal add 1.
  }

  command route(count normal, count emergency) : count
  {
    back normal add emergency.
  }

  module MissionQueue
  {
    public telemetry count backlog := 0.

    command MissionQueue(count seed) : voidspace
    {
      this.backlog := seed.
    }

    command enqueue(count tasks) : count
    {
      this.backlog := this.backlog add tasks.
      back this.backlog.
    }

    command enqueue(count tasks, count priority) : count
    {
      this.backlog := this.backlog add tasks add priority.
      back this.backlog.
    }
  }

  deploy MissionQueue q(5).

  telemetry count r1 := route(3).
  telemetry count r2 := route(3, 4).
  transmit r1.
  transmit r2.
  transmit q.enqueue(2).
  transmit q.enqueue(2, 1).

  telemetry count backlog := 77.
  verify (1 == 1)
  {
    telemetry count backlog := 500.
    transmit backlog.
  }

  transmit backlog.
}
success`,
  },
  modeAndAbort: {
  label: "Module 12: mode + abort",
  code: `mission ModeAbort launch
{
  mode MissionState
  {
    trajectory 1.
    trajectory 2.
    fallback 0.
  }

  telemetry count code := 1.
  transmit code.
  abort.
}
success`,
  },
  exhaustiveAllKeywords: {
  label: "Exhaustive: All Keywords and Tokens",
  code: `mission ExhaustiveAllKeywords launch
{
  $$ single-line comment
  $* multi-line
     comment *$

  alias count IntAlias.
  mode MissionState
  {
    trajectory 1.
    trajectory 2.
    fallback 0.
  }

  telemetry count a := 12.
  telemetry count b := 5.
  telemetry count i := 0.
  telemetry count r.
  telemetry real rf := 3.5.
  telemetry precise pd := 9.25.
  telemetry flag ok := 1.
  telemetry symbol msg := "Astro".
  telemetry IntAlias aliasVar := 10.
  telemetry count arr[5].
  fleet count dynFleet[3].
  limit count LIMITER := 99.

  command add_kw(count x, count y) : count
  {
    back x add y.
  }

  command mul_sym(count x, count y) :-> count
  {
    back x * y.
  }

  command noop() : voidspace
  {
    transmit "noop".
    back.
  }

  module BaseModule
  {
    public telemetry count base_id.

    command id() : count
    {
      back base_id.
    }
  }

  module ChildModule extends BaseModule
  {
    private telemetry symbol name.

    command who() : count
    {
      transmit this.
      back 7.
    }
  }

  deploy ChildModule ship.

  r := a add b.
  transmit r.
  dynFleet[0] := r.
  dynFleet[1] := dynFleet[0] add 1.
  transmit dynFleet[1].
  r := a + b.
  transmit r.
  r := a minus b.
  transmit r.
  r := a - b.
  transmit r.
  r := a mul b.
  transmit r.
  r := a * b.
  transmit r.
  r := a divide b.
  transmit r.
  r := a / b.
  transmit r.
  r := a mod b.
  transmit r.
  r := a ** 2.
  transmit r.

  verify ((a > b) AND (b > 0))
  {
    broadcast "AND true".
  }
  else_verify ((a == b) OR (b == 0))
  {
    alarm "edge".
  }
  otherwise
  {
    transmit "fallback verify".
  }

  verify (NOT (a < b))
  {
    transmit "NOT true".
  }

  verify ((a > 0) XOR (b < 0))
  {
    transmit "XOR true".
  }

  arr[0] := a.
  arr[1] := b.
  arr[2] := LIMITER.
  transmit arr[1].

  orbit (i < 6)
  {
    i := i add 1.
    verify (i == 2)
    {
      coast.
    }
    transmit i.
    verify (i == 4)
    {
      stage_sep.
    }
  }

  orbit while (i < 7)
  {
    transmit i.
    i := i add 1.
  }

  orbit times (i : 0 : 3)
  {
    transmit i.
  }

  scenario (i)
  {
    trajectory 1:
      transmit "case one".
      stage_sep.
    trajectory 3:
      transmit "case three".
      stage_sep.
    fallback:
      transmit "default case".
  }

  r := root(a).
  transmit r.
  r := flr(rf).
  transmit r.
  r := ceil(rf).
  transmit r.
  r := abs(-22).
  transmit r.
  r := logarithm(1).
  transmit r.
  r := sine(0).
  transmit r.
  r := cosine(0).
  transmit r.
  r := tan(0).
  transmit r.
  r := asine(0).
  transmit r.
  r := acosine(1).
  transmit r.
  r := atan(1).
  transmit r.
  r := prime(17).
  transmit r.

  r := add_kw(3, 4).
  transmit r.
  r := mul_sym(3, 4).
  transmit r.
  noop().

  receive r.
  transmit r.

  wait 1 tick.

  abort.
}
success`,
  },
};

const generatedExamples: Record<string, { label: string; code: string }> = Object.fromEntries(
  Object.entries(playgroundExamples).map(([key, value]) => [
    key,
    {
      label: value.label,
      code: value.code,
    },
  ]),
);

const examples: Record<string, { label: string; code: string }> =
  Object.keys(generatedExamples).length > 0 ? generatedExamples : legacyExamples;

export default function PlaygroundPage() {
  const [activeTab, setActiveTab] = useState<PlaygroundTab>("output");
  const [selectedExample, setSelectedExample] = useState<keyof typeof examples>("basics");
  const codeRef = useRef(examples.basics.code);
  const [editorExternalCode, setEditorExternalCode] = useState(examples.basics.code);
  const [split, setSplit] = useState(60);
  const [running, setRunning] = useState(false);
  const [programInput, setProgramInput] = useState("");
  const [output, setOutput] = useState("");
  const [tokens, setTokens] = useState("");
  const [ir, setIr] = useState("");
  const [cCode, setCCode] = useState("");
  const [error, setError] = useState("");
  const [stdout, setStdout] = useState("");
  const [stderr, setStderr] = useState("");
  const [userMessage, setUserMessage] = useState("");
  const [diagnostics, setDiagnostics] = useState<PlaygroundDiagnostic[]>([]);
  const [jumpToLine, setJumpToLine] = useState<number | null>(null);
  const [line, setLine] = useState(1);
  const [column, setColumn] = useState(1);
  const [lastRunMs, setLastRunMs] = useState<number | null>(null);
  const [compilerReady, setCompilerReady] = useState<boolean | null>(null);
  const [compilerStatusText, setCompilerStatusText] = useState("Checking compiler...");
  const [editorExternalVersion, setEditorExternalVersion] = useState(0);
  const splitContainerRef = useRef<HTMLDivElement>(null);
  const cursorRafRef = useRef<number | null>(null);
  const latestCursorRef = useRef<{ line: number; column: number }>({ line: 1, column: 1 });

  const exampleOptions = useMemo(
    () => Object.entries(examples).map(([key, value]) => ({ key, label: value.label })),
    [],
  );

  const refreshCompilerHealth = useCallback(async () => {
    try {
      const response = await fetch("/api/health", { cache: "no-store" });
      const data = (await response.json()) as HealthResponse;
      const ready = response.ok && data.compilerReady;

      setCompilerReady(ready);
      if (ready) {
        setCompilerStatusText(`Compiler online (${data.compilerPath?.split(/[\\/]/).pop() ?? "binary"})`);
      } else {
        setCompilerStatusText(data.message ?? "Compiler offline");
      }
    } catch {
      setCompilerReady(false);
      setCompilerStatusText("Health check failed");
    }
  }, []);

  useEffect(() => {
    refreshCompilerHealth();
  }, [refreshCompilerHealth]);

  const handleRun = useCallback(async () => {
    if (compilerReady === false) {
      setError(`Compiler unavailable: ${compilerStatusText}`);
      setActiveTab("errors");
      return;
    }

    const receiveCount = countReceiveStatements(codeRef.current);
    const resolvedInput = promptRuntimeInput(programInput, receiveCount);
    if (resolvedInput === null) {
      setUserMessage("Run canceled before execution.");
      return;
    }
    if (resolvedInput !== programInput) {
      setProgramInput(resolvedInput);
    }

    setRunning(true);
    setError("");
    setActiveTab("output");
    const startedAt = performance.now();

    try {
      const response = await fetch("/api/compile", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ code: codeRef.current, input: resolvedInput }),
      });

      const data = (await response.json()) as CompileResponse;

      setOutput(data.output ?? "");
      setTokens(data.tokens ?? "");
      setIr(data.ir ?? "");
      setCCode(data.cCode ?? "");
      setStdout(data.stdout ?? "");
      setStderr(data.stderr ?? "");
      setUserMessage(data.userMessage ?? "");
      setDiagnostics(data.diagnostics ?? []);

      if (!response.ok || data.error) {
        setError(data.error ?? "Compilation failed");
        setActiveTab("errors");
      } else {
        setError("");
        setActiveTab("output");
      }
    } catch (compileError) {
      const message = compileError instanceof Error ? compileError.message : "Compilation failed";
      setError(message);
      setUserMessage("The compile request failed before diagnostics were produced. Please retry.");
      setDiagnostics([]);
      setActiveTab("errors");
    } finally {
      setRunning(false);
      setLastRunMs(Math.round(performance.now() - startedAt));
    }
  }, [compilerReady, compilerStatusText, programInput]);

  useEffect(() => {
    const onKeyDown = (event: KeyboardEvent) => {
      if ((event.ctrlKey || event.metaKey) && event.key === "Enter") {
        event.preventDefault();
        if (!running) {
          handleRun();
        }
      }
    };

    window.addEventListener("keydown", onKeyDown);
    return () => window.removeEventListener("keydown", onKeyDown);
  }, [handleRun, running]);

  const handleCodeChange = useCallback((nextCode: string) => {
    codeRef.current = nextCode;
  }, []);

  const handleClear = useCallback(() => {
    const cleared = "";
    codeRef.current = cleared;
    setEditorExternalCode(cleared);
    setEditorExternalVersion((value) => value + 1);
    setOutput("");
    setTokens("");
    setIr("");
    setCCode("");
    setError("");
    setStdout("");
    setStderr("");
    setUserMessage("");
    setDiagnostics([]);
    setProgramInput("");
    setJumpToLine(null);
    setLastRunMs(null);
    setActiveTab("output");
  }, []);

  const handleSelectExample = useCallback((key: string) => {
    if (!(key in examples)) {
      return;
    }

    const typedKey = key as keyof typeof examples;
    const nextCode = examples[typedKey].code;
    setSelectedExample(typedKey);
    codeRef.current = nextCode;
    setEditorExternalCode(nextCode);
    setEditorExternalVersion((value) => value + 1);
    setActiveTab("output");
  }, []);

  const handleCursorChange = useCallback((nextLine: number, nextColumn: number) => {
    latestCursorRef.current = { line: nextLine, column: nextColumn };

    if (cursorRafRef.current !== null) {
      return;
    }

    cursorRafRef.current = requestAnimationFrame(() => {
      cursorRafRef.current = null;
      const { line: currentLine, column: currentColumn } = latestCursorRef.current;
      setLine((previous) => (previous === currentLine ? previous : currentLine));
      setColumn((previous) => (previous === currentColumn ? previous : currentColumn));
    });
  }, []);

  useEffect(() => {
    return () => {
      if (cursorRafRef.current !== null) {
        cancelAnimationFrame(cursorRafRef.current);
      }
    };
  }, []);

  const handleDividerMouseDown = useCallback((event: React.MouseEvent<HTMLButtonElement>) => {
    event.preventDefault();

    const onMouseMove = (moveEvent: MouseEvent) => {
      const host = splitContainerRef.current;
      if (!host) {
        return;
      }

      const rect = host.getBoundingClientRect();
      const nextSplit = ((moveEvent.clientX - rect.left) / rect.width) * 100;
      setSplit(Math.min(78, Math.max(32, nextSplit)));
    };

    const onMouseUp = () => {
      window.removeEventListener("mousemove", onMouseMove);
      window.removeEventListener("mouseup", onMouseUp);
    };

    window.addEventListener("mousemove", onMouseMove);
    window.addEventListener("mouseup", onMouseUp);
  }, []);

  const handleJumpHandled = useCallback(() => {
    setJumpToLine(null);
  }, []);

  const handleJumpToLine = useCallback((lineNumber: number) => {
    setJumpToLine(lineNumber);
  }, []);

  return (
    <div className="relative flex min-h-screen flex-col overflow-hidden bg-[#10131e] text-[#f6f6f8]">
      <div className="pointer-events-none absolute inset-0">
        <div className="absolute -left-24 top-16 h-72 w-72 rounded-full bg-[#5166ff]/20 blur-3xl" />
        <div className="absolute -right-24 -top-8 h-80 w-80 rounded-full bg-[#4dc5ff]/16 blur-3xl" />
        <div className="absolute bottom-12 left-1/3 h-56 w-56 rounded-full bg-[#1f2f68]/24 blur-3xl" />
      </div>

      <Navbar />
      <PlaygroundToolbar
        onRun={handleRun}
        onClear={handleClear}
        running={running}
        selectedExample={selectedExample}
        examples={exampleOptions}
        onSelectExample={handleSelectExample}
        compilerReady={compilerReady}
        compilerStatusText={compilerStatusText}
        onRefreshCompilerHealth={refreshCompilerHealth}
      />

      <section className="relative z-10 border-b border-white/10 bg-[#121833]/85 px-3 py-2 md:px-4" aria-label="Program input panel">
        <div className="mb-1 flex items-center justify-between gap-2 text-[11px] uppercase tracking-[0.08em] text-[#99a3d1]">
          <label htmlFor="playground-input">Program Input (stdin)</label>
          <span className="text-[#7983b4]">One value per line for each receive statement</span>
        </div>
        <textarea
          id="playground-input"
          value={programInput}
          onChange={(event) => setProgramInput(event.target.value)}
          placeholder={"42\nhello\n3.14"}
          spellCheck={false}
          className="h-20 w-full resize-y rounded-md border border-white/15 bg-[#0d1228] px-3 py-2 font-mono text-sm text-[#d7ddff] outline-none transition-colors focus:border-[#6074ff]"
        />
      </section>

      <main className="relative z-10 flex flex-1 overflow-hidden" aria-label="AstroScript playground workspace">
        <PlaygroundSidebar />

        <div
          ref={splitContainerRef}
          className="flex flex-1 flex-col overflow-hidden md:flex-row"
          style={{ "--editor-split": `${split}%`, "--output-split": `${100 - split}%` } as CSSProperties}
        >
          <div className="w-full md:h-full md:basis-(--editor-split)">
            <CodeEditorPanel
              externalCode={editorExternalCode}
              externalCodeVersion={editorExternalVersion}
              onChange={handleCodeChange}
              onCursorChange={handleCursorChange}
              diagnostics={diagnostics}
              jumpToLine={jumpToLine}
              onJumpHandled={handleJumpHandled}
            />
          </div>

          <EditorDivider onMouseDown={handleDividerMouseDown} />

          <div className="min-h-72 w-full flex-1 md:h-full md:min-h-0 md:basis-(--output-split)">
            <OutputPanel
              activeTab={activeTab}
              onChangeTab={setActiveTab}
              running={running}
              output={output}
              tokens={tokens}
              ir={ir}
              cCode={cCode}
              error={error}
              stdout={stdout}
              stderr={stderr}
              userMessage={userMessage}
              diagnostics={diagnostics}
              onJumpToLine={handleJumpToLine}
            />
          </div>
        </div>
      </main>

      <StatusBar running={running} line={line} column={column} lastRunMs={lastRunMs} compilerReady={compilerReady} />
    </div>
  );
}
