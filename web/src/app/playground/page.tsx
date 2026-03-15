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

export default function PlaygroundPage() {
  const [activeTab, setActiveTab] = useState<PlaygroundTab>("output");
  const [split, setSplit] = useState(62);
  const [editorResetKey, setEditorResetKey] = useState(0);
  const splitContainerRef = useRef<HTMLDivElement>(null);

  const handleRun = useCallback(() => {
    setActiveTab("output");
  }, []);

  const handleClear = useCallback(() => {
    setEditorResetKey((value) => value + 1);
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
      setSplit(Math.min(75, Math.max(35, nextSplit)));
    };

    const onMouseUp = () => {
      window.removeEventListener("mousemove", onMouseMove);
      window.removeEventListener("mouseup", onMouseUp);
    };

    window.addEventListener("mousemove", onMouseMove);
    window.addEventListener("mouseup", onMouseUp);
  }, []);

  const codeLines = useMemo(
    () => [
      {
        number: 1,
        content: (
          <>
            <span className="text-[#b47eff]">mission</span> <span className="text-[#84a9ff]">Apollo11</span> {"{"}
          </>
        ),
      },
      {
        number: 2,
        content: <span className="text-[#646d92]">    {"// Initialize propulsion systems"}</span>,
      },
      {
        number: 3,
        content: (
          <>
            <span className="text-[#b47eff]">    variable</span> <span className="text-[#84a9ff]">thrust</span> = <span className="text-[#f2aa5f]">100.0</span>;
          </>
        ),
      },
      {
        number: 4,
        content: (
          <>
            <span className="text-[#b47eff]">    variable</span> <span className="text-[#84a9ff]">target</span> = <span className="text-[#7de894]">&quot;Moon&quot;</span>;
          </>
        ),
      },
      { number: 5, content: <span> </span> },
      {
        number: 6,
        content: (
          <>
            <span className="text-[#b47eff]">    launch</span> <span className="text-[#f5de67]">sequence</span>() {" {"}
          </>
        ),
      },
      {
        number: 7,
        content: (
          <>
            <span className="text-[#7ad3ff]">        log</span>(<span className="text-[#7de894]">&quot;Ignition start...&quot;</span>);
          </>
        ),
      },
      {
        number: 8,
        content: (
          <>
            <span className="text-[#b47eff]">        if</span> (thrust &gt; <span className="text-[#f2aa5f]">0</span>) {" {"}
          </>
        ),
      },
      {
        number: 9,
        content: (
          <>
            <span className="text-[#7ad3ff]">            execute</span> propulsion.start();
          </>
        ),
      },
      { number: 10, content: <span>        {"}"}</span> },
      { number: 11, content: <span>    {"}"}</span> },
      { number: 12, content: <span>{"}"}</span> },
      { number: 13, content: <span> </span> },
      {
        number: 14,
        content: (
          <>
            <span className="text-[#b47eff]">mission</span> <span className="text-[#84a9ff]">HelloGalaxy</span> {"{"}
          </>
        ),
      },
      {
        number: 15,
        content: (
          <>
            <span className="text-[#b47eff]">    launch</span> <span className="text-[#f5de67]">main</span>() {" {"}
          </>
        ),
      },
      {
        number: 16,
        content: (
          <>
            <span className="text-[#7ad3ff]">        log</span>(<span className="text-[#7de894]">&quot;Hello Galaxy! Ready for orbit.&quot;</span>);
          </>
        ),
      },
      { number: 17, content: <span>    {"}"}</span> },
      { number: 18, content: <span>{"}"}</span> },
    ],
    [],
  );

  return (
    <div className="relative flex min-h-screen flex-col overflow-hidden bg-[#111121] text-[#f6f6f8]">
      <div className="pointer-events-none absolute inset-0">
        <div className="absolute -left-24 top-20 h-72 w-72 rounded-full bg-[#2c2ce2]/20 blur-3xl" />
        <div className="absolute -right-22.5 -top-5 h-80 w-80 rounded-full bg-[#2a3b90]/20 blur-3xl" />
        <div className="absolute bottom-16 left-1/3 h-56 w-56 rounded-full bg-[#1e275b]/20 blur-3xl" />
      </div>

      <PlaygroundNavbar />
      <PlaygroundToolbar onRun={handleRun} onClear={handleClear} />

      <main className="relative z-10 flex flex-1 overflow-hidden" aria-label="AstroScript playground workspace">
        <PlaygroundSidebar />

        <div
          ref={splitContainerRef}
          className="flex flex-1 flex-col overflow-hidden md:flex-row"
          style={{ "--editor-split": `${split}%`, "--output-split": `${100 - split}%` } as CSSProperties}
        >
          <div className="w-full md:h-full md:basis-[var(--editor-split)]">
            <CodeEditorPanel key={editorResetKey} lines={codeLines} />
          </div>

          <EditorDivider onMouseDown={handleDividerMouseDown} />

          <div className="min-h-0 w-full flex-1 md:h-full md:basis-[var(--output-split)]">
            <OutputPanel activeTab={activeTab} onChangeTab={setActiveTab} />
          </div>
        </div>
      </main>

      <StatusBar />
    </div>
  );
}
