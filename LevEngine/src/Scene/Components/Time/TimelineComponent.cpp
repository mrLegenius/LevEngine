#include "levpch.h"
#include "TimelineComponent.h"

#include "Kernel/Time/Timeline.h"
#include "Kernel/Time/TimelineParameters.h"

namespace LevEngine
{
    void TimelineComponent::Init()
    {
        const auto parameters = CreateRef(TimelineParameters{playOnInit, isLooping, duration});
        timeline = new Timeline(parameters);
    }

    double TimelineComponent::GetElapsedTime() const
    {
        if (timeline != nullptr)
        {
            return timeline->GetTimeSinceStartup();
        }

        return 0.0f;
    }

    double TimelineComponent::GetDuration() const
    {
        return duration;
    }

    void TimelineComponent::SetDuration(double duration)
    {
        this->duration = duration;
    }
}
