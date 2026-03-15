"use client";

import { type CSSProperties, useCallback, useMemo, useRef, useState } from "react";
import { CodeEditorPanel } from "@/components/playground/CodeEditorPanel";
import { EditorDivider } from "@/components/playground/EditorDivider";
import { OutputPanel } from "@/components/playground/OutputPanel";
import { PlaygroundNavbar } from "@/components/playground/PlaygroundNavbar";
import { PlaygroundSidebar } from "@/components/playground/PlaygroundSidebar";
import { PlaygroundToolbar } from "@/components/playground/PlaygroundToolbar";
import { StatusBar } from "@/components/playground/StatusBar";

type PlaygroundTab = "output" | "tokens" | "ir" | "errors";

type CompileResponse = {
  output?: string;
  tokens?: string;
  ir?: string;
  error?: string;
  stdout?: string;
  stderr?: string;
};

const examples: Record<string, { label: string; code: string }> = {
  basics: {
    label: "Mission Basics",
    code: `mission Apollo launch
{
    telemetry count astronauts := 3.

    verify (astronauts > 0)
    {
        broadcast("Mission ready").
    }

    orbit (astronauts > 0)
    {
        astronauts := astronauts minus 1.
    }
}
success`,
  },
  arithmetic: {
    label: "Arithmetic and Output",
    code: `mission Calculator launch
{
    telemetry count x := 12.
    telemetry count y := 8.
    telemetry count sum := x add y.
    telemetry count diff := x minus y.

    transmit sum.
    transmit diff.
}
success`,
  },
  controlFlow: {
    label: "Control Flow",
    code: `mission ControlFlow launch
{
    telemetry count fuel := 5.

    verify (fuel > 3)
    {
        broadcast("Launch condition satisfied").
    }
    otherwise
    {
        alarm("Fuel below launch threshold").
    }
}
success`,
  },
};

export default function PlaygroundPage() {
  const [activeTab, setActiveTab] = useState<PlaygroundTab>("output");
  const [selectedExample, setSelectedExample] = useState<keyof typeof examples>("basics");
  const [code, setCode] = useState(examples.basics.code);
  const [split, setSplit] = useState(60);
  const [running, setRunning] = useState(false);
  const [output, setOutput] = useState("");
  const [tokens, setTokens] = useState("");
  const [ir, setIr] = useState("");
  const [error, setError] = useState("");
  const [stdout, setStdout] = useState("");
  const [stderr, setStderr] = useState("");
  const [line, setLine] = useState(1);
  const [column, setColumn] = useState(1);
  const [lastRunMs, setLastRunMs] = useState<number | null>(null);
  const splitContainerRef = useRef<HTMLDivElement>(null);

  const exampleOptions = useMemo(
    () => Object.entries(examples).map(([key, value]) => ({ key, label: value.label })),
    [],
  );

  const handleRun = useCallback(async () => {
    setRunning(true);
    setError("");
    setActiveTab("output");
    const startedAt = performance.now();

    try {
      const response = await fetch("/api/compile", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ code }),
      });

      const data = (await response.json()) as CompileResponse;

      setOutput(data.output ?? "");
      setTokens(data.tokens ?? "");
      setIr(data.ir ?? "");
      setStdout(data.stdout ?? "");
      setStderr(data.stderr ?? "");

      if (!response.ok || data.error) {
        setError(data.error ?? "Compilation failed");
        setActiveTab("errors");
      } else {
        setActiveTab("output");
      }
    } catch (compileError) {
      const message = compileError instanceof Error ? compileError.message : "Compilation failed";
      setError(message);
      setActiveTab("errors");
    } finally {
      setRunning(false);
      setLastRunMs(Math.round(performance.now() - startedAt));
    }
  }, [code]);

  const handleClear = useCallback(() => {
    setCode("");
    setOutput("");
    setTokens("");
    setIr("");
    setError("");
    setStdout("");
    setStderr("");
    setLastRunMs(null);
    setActiveTab("output");
  }, []);

  const handleSelectExample = useCallback((key: string) => {
    if (!(key in examples)) {
      return;
    }

    const typedKey = key as keyof typeof examples;
    setSelectedExample(typedKey);
    setCode(examples[typedKey].code);
    setActiveTab("output");
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

  return (
    <div className="relative flex min-h-screen flex-col overflow-hidden bg-[#111121] text-[#f6f6f8]">
      <div className="pointer-events-none absolute inset-0">
        <div className="absolute -left-24 top-20 h-72 w-72 rounded-full bg-[#2c2ce2]/20 blur-3xl" />
        <div className="absolute -right-24 -top-5 h-80 w-80 rounded-full bg-[#2a3b90]/20 blur-3xl" />
        <div className="absolute bottom-16 left-1/3 h-56 w-56 rounded-full bg-[#1e275b]/20 blur-3xl" />
      </div>

      <PlaygroundNavbar />
      <PlaygroundToolbar
        onRun={handleRun}
        onClear={handleClear}
        running={running}
        selectedExample={selectedExample}
        examples={exampleOptions}
        onSelectExample={handleSelectExample}
      />

      <main className="relative z-10 flex flex-1 overflow-hidden" aria-label="AstroScript playground workspace">
        <PlaygroundSidebar />

        <div
          ref={splitContainerRef}
          className="flex flex-1 flex-col overflow-hidden md:flex-row"
          style={{ "--editor-split": `${split}%`, "--output-split": `${100 - split}%` } as CSSProperties}
        >
          <div className="w-full md:h-full md:basis-(--editor-split)">
            <CodeEditorPanel code={code} onChange={setCode} onCursorChange={(l, c) => { setLine(l); setColumn(c); }} />
          </div>

          <EditorDivider onMouseDown={handleDividerMouseDown} />

          <div className="min-h-0 w-full flex-1 md:h-full md:basis-(--output-split)">
            <OutputPanel
              activeTab={activeTab}
              onChangeTab={setActiveTab}
              running={running}
              output={output}
              tokens={tokens}
              ir={ir}
              error={error}
              stdout={stdout}
              stderr={stderr}
            />
          </div>
        </div>
      </main>

      <StatusBar running={running} line={line} column={column} lastRunMs={lastRunMs} />
    </div>
  );
}
