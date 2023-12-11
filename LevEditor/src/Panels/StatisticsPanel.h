#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class StatisticsPanel final : public Panel
    {
    public:
        StatisticsPanel();
        
    protected:
        String GetName() override;
        void DrawContent() override;
        
    private:
        static void DrawAverageStat(const String& label, const Statistic& stat);
        static void DrawStatPerSecondStat(const String& label, const Statistic& stat);
    };
}
