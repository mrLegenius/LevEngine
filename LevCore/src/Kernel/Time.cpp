#include "Time.h"
using time_clock = std::chrono::high_resolution_clock;

namespace LevEngine
{
	Timestep Time::s_DeltaTime = 0;
    std::chrono::time_point<time_clock> Time::s_StartupTime =  std::chrono::time_point<time_clock>{ };

	Timestep Time::GetDeltaTime()
	{
		return s_DeltaTime;
	}

    Timestep Time::GetTimeSinceStartup()
    {
        auto timeSinceStartup = time_clock::now() - s_StartupTime;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStartup);
        return (float)milliseconds.count() / 1000.0f;
    }
}
