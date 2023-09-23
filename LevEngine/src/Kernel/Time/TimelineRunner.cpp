#include "levpch.h"
#include "TimelineRunner.h"

#include "Timeline.h"

Vector<LevEngine::Timeline*> LevEngine::TimelineRunner::s_Timelines;

void LevEngine::TimelineRunner::AddTimeline(Timeline* timeline)
{
    s_Timelines.emplace_back(timeline);
}

void LevEngine::TimelineRunner::OnUpdate(float deltaTime)
{
    for (const auto timeline : s_Timelines)
    {
        if (timeline->IsPlaying())
        {
            timeline->OnUpdate(deltaTime);   
        }
    }
}
