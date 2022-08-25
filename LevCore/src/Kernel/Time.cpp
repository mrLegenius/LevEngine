#include "Time.h"

namespace LevEngine
{
	Timestep Time::s_DeltaTime = 0;
	
	Timestep Time::GetDeltaTime()
	{
		return s_DeltaTime;
	}
}
