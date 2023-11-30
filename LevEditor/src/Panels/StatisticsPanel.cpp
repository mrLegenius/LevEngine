#include "pch.h"
#include "StatisticsPanel.h"

namespace LevEngine::Editor
{
    StatisticsPanel::StatisticsPanel() { }

    String StatisticsPanel::GetName() { return "Statistics"; }

    void StatisticsPanel::DrawContent()
    {
        DrawStatPerSecondStat("FPS", App::Get().GetFrameStat());
        
        DrawAverageStat("Frame Time", App::Get().GetFrameStat());
        DrawAverageStat("GPU Time", App::Renderer().GetFrameStatistic());
        
        ImGui::Indent();
        DrawAverageStat("Environment", App::Renderer().GetEnvironmentStatistic());
        DrawAverageStat("Deferred Geometry", App::Renderer().GetDeferredGeometryStatistic());
        DrawAverageStat("Deferred Lighting", App::Renderer().GetDeferredLightingStatistic());
        DrawAverageStat("Deferred Transparent", App::Renderer().GetDeferredTransparentStatistic());
        DrawAverageStat("Post Processing", App::Renderer().GetPostProcessingStatistic());
        DrawAverageStat("Particles", App::Renderer().GetParticlesStatistic());
        ImGui::Unindent();
    }

    void StatisticsPanel::DrawAverageStat(const String& label, const Statistic& stat)
    {
        const auto statAverage = ToString(static_cast<float>(stat.GetAverage()));
        ImGui::Text("%s: %.2f ms", label.c_str(), stat.GetAverage());
    }

    void StatisticsPanel::DrawStatPerSecondStat(const String& label, const Statistic& stat)
    {
        const auto statAverage = ToString(static_cast<float>(stat.GetPerSecond()));
        ImGui::Text("%s: %.2f", label.c_str(), stat.GetPerSecond());
    }
}
