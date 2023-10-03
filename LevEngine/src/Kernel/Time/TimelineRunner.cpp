#include "levpch.h"
#include "TimelineRunner.h"

#include "Timeline.h"

namespace LevEngine
{
    Vector<Ref<Timeline>> TimelineRunner::s_Timelines;

    void TimelineRunner::AddTimeline(Ref<Timeline> timeline)
    {
        s_Timelines.emplace_back(timeline);
    }

    void TimelineRunner::OnUpdate(float deltaTime)
    {
        for (const auto timeline : s_Timelines)
        {
            if (timeline->IsPlaying())
            {
                timeline->OnUpdate(deltaTime);   
            }
        }
    }
}

