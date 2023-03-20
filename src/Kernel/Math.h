#pragma once
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

namespace LevEngine
{
	class Math
	{
	public:

		template<typename T>
		static T Max(T a, T b) { return ((a) > (b)) ? (a) : (b); }

		static float MaxElement(const Vector3 vector)
		{
			return Max(Max(vector.x, vector.y), vector.z);
		}
	};
}
