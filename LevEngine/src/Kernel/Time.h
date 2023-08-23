#pragma once
#include <chrono>

#include "Timestep.h"

namespace LevEngine
{
	class Application;

	class Time
	{
	public:
		friend Application;

		static void SetDeltaTime(float deltaTime);
		static Timestep GetDeltaTime();
		static Timestep GetTimeSinceStartup();
	private:
		
		static Timestep s_DeltaTime;
		static std::chrono::time_point<std::chrono::high_resolution_clock> s_StartupTime;
	};
}
