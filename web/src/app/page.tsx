import FeatureCards from "@/components/FeatureCards";
import Footer from "@/components/Footer";
import HeroSection from "@/components/HeroSection";
import Navbar from "@/components/Navbar";
import PipelineSection from "@/components/PipelineSection";

export default function Home() {
  return (
    <div className="relative min-h-screen overflow-x-hidden bg-background-dark text-white">
      <div className="pointer-events-none fixed inset-0 -z-20 bg-[radial-gradient(circle_at_20%_0%,rgba(53,70,212,0.35),transparent_45%),radial-gradient(circle_at_80%_8%,rgba(25,33,86,0.42),transparent_48%),linear-gradient(180deg,#0b0f2e_0%,#0a0d26_40%,#0a0c24_100%)]" />
      <div
        className="pointer-events-none fixed inset-0 -z-10 bg-cover bg-center opacity-10 mix-blend-screen blur-[2px]"
        style={{ backgroundImage: "url('https://cdn.esawebb.org/archives/images/screen/weic2205a.jpg')" }}
      />
      <div className="pointer-events-none fixed inset-0 -z-10 bg-gradient-to-b from-[#0a1032]/78 via-[#090c24]/84 to-[#080a1e]/95" />

      <Navbar />
      <main>
        <HeroSection />
        <FeatureCards />
        <PipelineSection />
      </main>
      <Footer />
    </div>
  );
}
