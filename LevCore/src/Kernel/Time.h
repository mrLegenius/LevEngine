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
	private:
		static Timestep s_DeltaTime;
	};
}
