#include "pch.h"
#include "Time.h"
using time_clock = std::chrono::high_resolution_clock;

namespace LevEngine
{
    Timestep Time::s_DeltaTime = 0;
    std::chrono::time_point<time_clock> Time::s_StartupTime = std::chrono::time_point<time_clock>{ };

    Timestep Time::GetDeltaTime()
    {
        return s_DeltaTime;
    }

    Timestep Time::GetTimeSinceStartup()
    {
	    const auto timeSinceStartup = time_clock::now() - s_StartupTime;
	    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStartup);
        return static_cast<float>(milliseconds.count()) / 1000.0f;
    }

    void Time::SetDeltaTime(const float deltaTime)
    {
        s_DeltaTime = Timestep(deltaTime);
    }
}
