import DocsContent from "@/components/docs/DocsContent";
import DocsFooter from "@/components/docs/DocsFooter";
import DocsNavbar from "@/components/docs/DocsNavbar";
import DocsRightSidebar from "@/components/docs/DocsRightSidebar";
import DocsSidebar from "@/components/docs/DocsSidebar";

export default function DocsLayout({ children }: { children: React.ReactNode }) {
  return (
    <div className="relative min-h-screen overflow-x-clip bg-background text-foreground">
      <div className="docs-cosmic-bg pointer-events-none absolute inset-0" aria-hidden="true" />
      <div className="relative z-10">
        <DocsNavbar />

        <div className="mx-auto flex w-full max-w-400">
          <DocsSidebar />
          <div className="min-w-0 flex-1">
            <DocsContent>{children}</DocsContent>
            <DocsFooter />
          </div>
          <DocsRightSidebar />
        </div>
      </div>
    </div>
  );
}
