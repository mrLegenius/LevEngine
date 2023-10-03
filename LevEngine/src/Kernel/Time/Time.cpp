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
	    return Timestep(s_TimelineVariable->GetTimeSinceStartup());
    }

    void Time::Init(float deltaTimeFixed)
    {
        s_TimelineVariable = TimelineFactory::CreateTimeline(TimelineParameters{false, -1, 1.0});
        s_TimelineFixed = TimelineFactory::CreateTimeline(TimelineParameters{false, -1, 1.0});
        s_DeltaTimeFixed = Timestep(deltaTimeFixed);

        s_TimelineVariable->Play();
        s_TimelineFixed->Play();
    }

    void Time::SetDeltaTime(const float deltaTime)
    {
        s_DeltaTime = Timestep(deltaTime);
    }
}
