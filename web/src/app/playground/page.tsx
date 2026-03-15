"use client";

import { useState, useCallback } from "react";
import Editor from "@monaco-editor/react";
import Link from "next/link";

const DEFAULT_CODE = `mission HelloWorld launch {
    telemetry count x.
    telemetry count y.
    telemetry count sum.

    x := 5.
    y := 3.
    sum := x add y.

    transmit x.
    transmit y.
    transmit sum.

    verify (sum > 5) {
        transmit 888.
    } otherwise {
        transmit 0.
    }
} success
`;

type Tab = "output" | "tokens" | "ir" | "errors";

export default function PlaygroundPage() {
  const [code, setCode] = useState(DEFAULT_CODE);
  const [activeTab, setActiveTab] = useState<Tab>("output");
  const [output, setOutput] = useState("");
  const [errors, setErrors] = useState("");
  const [ir, setIr] = useState("");
  const [isRunning, setIsRunning] = useState(false);

  const handleRun = useCallback(async () => {
    setIsRunning(true);
    setOutput("");
    setErrors("");
    setIr("");

    try {
      const response = await fetch("/api/compile", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ code }),
      });

      const data = await response.json();

      if (data.error) {
        setErrors(data.error);
        setActiveTab("errors");
      } else {
        setOutput(data.output ?? "");
        setIr(data.ir ?? "");
        setActiveTab("output");
      }
    } catch (err) {
      setErrors(err instanceof Error ? err.message : "Failed to compile");
      setActiveTab("errors");
    } finally {
      setIsRunning(false);
    }
  }, [code]);

  const tabs: { id: Tab; label: string }[] = [
    { id: "output", label: "Output" },
    { id: "tokens", label: "Tokens" },
    { id: "ir", label: "Intermediate Code" },
    { id: "errors", label: "Errors" },
  ];

  const getTabContent = (tab: Tab) => {
    switch (tab) {
      case "output":
        return output || "Run your code to see output.";
      case "tokens":
        return "Token output will be available in a future version.";
      case "ir":
        return ir || "Run your code to see intermediate code.";
      case "errors":
        return errors || "No errors.";
    }
  };

  return (
    <div className="flex h-screen flex-col bg-gray-950 text-white">
      <nav className="flex items-center justify-between border-b border-gray-800 px-6 py-3">
        <Link href="/" className="text-lg font-bold tracking-tight">
          AstroScript
        </Link>
        <div className="flex items-center gap-4">
          <Link href="/docs" className="text-sm text-gray-400 hover:text-white transition-colors">
            Docs
          </Link>
          <button
            onClick={handleRun}
            disabled={isRunning}
            className="rounded-md bg-green-600 px-4 py-1.5 text-sm font-medium transition-colors hover:bg-green-500 disabled:opacity-50"
          >
            {isRunning ? "Running..." : "Run"}
          </button>
        </div>
      </nav>

      <div className="flex flex-1 overflow-hidden">
        {/* Left: Code Editor */}
        <div className="flex-1 border-r border-gray-800">
          <Editor
            height="100%"
            defaultLanguage="plaintext"
            theme="vs-dark"
            value={code}
            onChange={(value) => setCode(value ?? "")}
            options={{
              fontSize: 14,
              minimap: { enabled: false },
              scrollBeyondLastLine: false,
              padding: { top: 16 },
              fontFamily: "var(--font-geist-mono), monospace",
            }}
          />
        </div>

        {/* Right: Output Panel */}
        <div className="flex w-1/2 flex-col">
          <div className="flex border-b border-gray-800">
            {tabs.map((tab) => (
              <button
                key={tab.id}
                onClick={() => setActiveTab(tab.id)}
                className={`px-4 py-2 text-sm font-medium transition-colors ${
                  activeTab === tab.id
                    ? "border-b-2 border-blue-400 text-blue-400"
                    : "text-gray-500 hover:text-gray-300"
                }`}
              >
                {tab.label}
              </button>
            ))}
          </div>
          <pre className="flex-1 overflow-auto p-4 font-mono text-sm text-gray-300">
            {getTabContent(activeTab)}
          </pre>
        </div>
      </div>
    </div>
  );
}
