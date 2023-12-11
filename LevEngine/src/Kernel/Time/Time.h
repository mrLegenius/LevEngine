#pragma once
#include <chrono>

#include "Timestep.h"

namespace LevEngine
{
	class Timeline;
	class Application;

	class Time
	{
	public:
		friend Application;

		static void Init(float deltaTimeFixed);
		static void SetDeltaTime(float deltaTime);
		static Timestep GetScaledDeltaTime();
		static Timestep GetUnscaledDeltaTime();
		static Timestep GetFixedDeltaTime();
		static Timestep GetTimeSinceStartup();
		static uint64_t GetFrameNumber();
		
	private:
		static Timestep s_DeltaTime;
		static Timestep s_DeltaTimeFixed;
		static std::chrono::time_point<std::chrono::high_resolution_clock> s_StartupTime;
		static Ref<Timeline> s_TimelineVariable;
		static Ref<Timeline> s_TimelineFixed;
		static uint64_t s_FrameNumber;
	};
}
