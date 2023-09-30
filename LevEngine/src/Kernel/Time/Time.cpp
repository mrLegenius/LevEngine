#include "levpch.h"
#include "Time.h"
#include "Timeline.h"
#include "TimelineFactory.h"

using time_clock = std::chrono::high_resolution_clock;

namespace LevEngine
{
    Timestep Time::s_DeltaTime = Timestep(0.0f);
    Timestep Time::s_DeltaTimeFixed = Timestep(0.0f);
    std::chrono::time_point<time_clock> Time::s_StartupTime = std::chrono::time_point<time_clock>{ };
    Ref<Timeline> Time::s_TimelineVariable;
    Ref<Timeline> Time::s_TimelineFixed;

    Timestep Time::GetScaledDeltaTime()
    {
        return s_DeltaTime * s_TimelineVariable->GetTimeScale();
    }

    Timestep Time::GetUnscaledDeltaTime()
    {
        return s_DeltaTime;
    }

    Timestep Time::GetFixedDeltaTime()
    {
        return s_DeltaTimeFixed;
    }

    Timestep Time::GetTimeSinceStartup()
    {
	    const auto timeSinceStartup = time_clock::now() - s_StartupTime;
	    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStartup);
        return Timestep(static_cast<float>(milliseconds.count()) / 1000.0f);
    }

    void Time::Init(float deltaTimeFixed)
    {
        const Ref<TimelineParameters> timelineVariableParameters = CreateRef<TimelineParameters>({true, false, 0.0});
        const Ref<TimelineParameters> timelineFixedParameters = CreateRef<TimelineParameters>({true, false, 0.0});
        
        s_TimelineVariable = TimelineFactory::CreateTimeline(timelineVariableParameters);
        s_TimelineFixed = TimelineFactory::CreateTimeline(timelineFixedParameters);
        s_DeltaTimeFixed = Timestep(deltaTimeFixed);
    }

    void Time::SetDeltaTime(const float deltaTime)
    {
        s_DeltaTime = Timestep(deltaTime);
    }
}
