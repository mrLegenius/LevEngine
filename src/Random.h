#pragma once
#include <cstdlib>

namespace Random
{
	inline int Range(int min, int max)
	{
		return min + rand() % (max - min);
	}
}
