#pragma once

#include "Kernel/Core.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Renderer/Particles/BitonicSort.h"

namespace LevEngine
{
	class LEV_API Math
	{
	public:

		static constexpr float FloatEpsilon = 0.000001f;
		static constexpr float Pi = 3.141592653589793238463f;
		static constexpr float PiDiv2 = Pi / 2.0f;
		static constexpr float Pi2 = Pi * 2.0f;
		static constexpr float RadToDeg = 180.0f / 3.141592653589793238463f;
		static constexpr float DegToRad = 3.141592653589793238463f / 180.0f;

		template<typename T>
		static constexpr auto Max(const T a, const T b) { return std::max(a, b); }
		
		template<typename T>
		static constexpr auto Min(const T a, const T b) { return std::min(a, b); }

		static int Sign(const int a) { return ((a) >= (0)) ? (1) : (-1); }
		static float Sign(const float a) { return ((a) > (-FloatEpsilon)) ? (1.0f) : (-1.0f); }
		static double Sign(const double a) { return ((a) > (-FloatEpsilon)) ? (1.0) : (-1.0); }

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

		static float Abs(const float value) { return std::abs(value); }
		static int32_t Abs(const int32_t value) { return std::abs(value); }

		static float Floor(const float value) { return std::floor(value); }
		static float Ceil(const float value) { return std::ceil(value); }
		static float Sqrt(const float value) { return std::sqrt(value); }
		static float Pow(const float value, const float power) { return std::pow(value, power); }

		static float Saturate(const float value) { return Clamp(value, 0.0f, 1.0f); }

		// Where value sits between from and to, as 0 at from and 1 at to. Degenerate ranges give 0
		// rather than a division by zero, which is what a range of no width should read as: a
		// threshold the value is either side of, contributing nothing either way.
		static float InverseLerp(const float from, const float to, const float value)
		{
			const float range = to - from;
			if (Abs(range) < FloatEpsilon) return 0.0f;

			return (value - from) / range;
		}

		// The same 0..1 ramp with zero slope at both ends. Anything that blends by a threshold wants
		// this rather than a linear ramp: a linear one leaves a visible crease where the blend
		// starts and stops, because the derivative jumps.
		static float Smoothstep(const float from, const float to, const float value)
		{
			const float t = Saturate(InverseLerp(from, to, value));
			return t * t * (3.0f - 2.0f * t);
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

		template<typename T>
		static float Log2(const T numElements)
		{
			return std::log2f(numElements);
		}
	};
}
