#pragma once
#include "Timestep.h"

namespace LevEngine
{
	class Application;

	class Time
	{
		friend Application;
	public:
		static Timestep GetDeltaTime();
        static Timestep GetTimeSinceStartup();
	private:
		static Timestep s_DeltaTime;
        static std::chrono::time_point<std::chrono::high_resolution_clock> s_StartupTime;
	};
}
