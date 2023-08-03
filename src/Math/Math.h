#pragma once

namespace LevEngine
{
	class Math
	{
	public:

		static constexpr float FloatEpsilon = 0.000001;

		template<typename T>
		static T Max(const T a, const T b) { return ((a) > (b)) ? (a) : (b); }

		template<typename T>
		static T Min(const T a, const T b) { return ((a) < (b)) ? (a) : (b); }

		static float MaxElement(const Vector3 vector)
		{
			return Max(Max(vector.x, vector.y), vector.z);
		}

		static float Lerp(const float a, const float b, const float t)
		{
			return a + t * (b - a);
		}

		template<typename T>
		static T Clamp(const T value, const T from, const T to)
		{
			return Min(Max(value, from), to);
		}

		static bool IsZero(const float value)
		{
			return IsEqual(value, 0.0f);
		}

		static bool IsEqual(const float a, const float b)
		{
			return std::abs(a - b) < FloatEpsilon;
		}
	};
}
