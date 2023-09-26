#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace LevEngine
{
	class Math
	{
	public:

		static constexpr float FloatEpsilon = 0.000001f;
		static constexpr float Pi = 3.141592653589793238463f;
		static constexpr float RadToDeg = 180.0f / 3.141592653589793238463f;
		static constexpr float DegToRad = 3.141592653589793238463f / 180.0f;

		template<typename T>
		static constexpr auto Max(const T a, const T b) { return std::max(a, b); }
		
		template<typename T>
		static constexpr auto Min(const T a, const T b) { return std::min(a, b); }

		static constexpr float MaxElement(const Vector3 vector)
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

		template<typename T>
		constexpr static T ToDegrees(const T value) { return value * RadToDeg; }

		template<typename T>
		constexpr static T ToRadians(const T value) { return value * DegToRad; }

		static bool DecomposeTransform(Matrix& transform,
		                               Vector3& outPosition,
		                               Quaternion& outRotation,
		                               Vector3& outScale)
		{
			return  transform.Decompose(outScale, outRotation, outPosition);
		}
	};
}
