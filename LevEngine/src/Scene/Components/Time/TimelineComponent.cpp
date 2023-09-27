#include "levpch.h"
#include "TimelineComponent.h"

#include "Kernel/Time/Timeline.h"
#include "Kernel/Time/TimelineParameters.h"

TimelineComponent::TimelineComponent()
{
    
}

void TimelineComponent::Init()
{
    const auto parameters = LevEngine::CreateRef(TimelineParameters{true, isLooping, duration});
    timeline = new LevEngine::Timeline(parameters);
}

float TimelineComponent::GetElapsedTime() const
{
    if (timeline != nullptr)
    {
        return timeline->GetTimeSinceStartup();
    }

    return 0.0f;
}
