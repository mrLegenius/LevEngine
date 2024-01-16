#pragma once
#include <random>

#include "Math.h"
#include "Color.h"

namespace LevEngine
{
struct Random
{
	static void Init()
	{
		std::random_device os_seed;
		const U32 seed = os_seed();

		generator = CreateScope<Engine>(seed);
	}

#pragma region Float

	/**
	 * \return float in [0, 1]
	 */
	static float Float()
	{
		return Float(0.0f, 1.0f);
	}

	/**
	 * \return float in [min, max]
	 */
	static float Float(const float min, const float max)
	{
		std::uniform_real_distribution distribution{min, max};
		return distribution(*generator);
	}

#pragma endregion

#pragma region Int
	/**
	 * \return int in [min, max)
	 */
	static int Int(const int min, const int max)
	{
		std::uniform_int_distribution distribution{min, max-1};
		return distribution(*generator);
	}

#pragma endregion

#pragma region Bool

	/**
	 * \return true or false with 50% chance
	 */
	static bool Bool()
	{
		std::uniform_int_distribution distribution{0, 1};
		return distribution(*generator);
	}

#pragma endregion

#pragma region Vector3

	/**
	 * \return Vector3 with each element in [0, 1]
	 */
	static Vector3 Vec3()
	{
		return Vector3{ Float(), Float(), Float() };
	}

	/**
	 * \return Vector3 with each element in [min, max]
	 */
	static Vector3 Vec3(const float min, const float max)
	{
		return Vector3{ Float(min, max), Float(min, max), Float(min, max) };
	}

	/**
	 * \return Vector3 with x = [a.x, b.x], y = [a.y, b.y], z = [a.z, b.z]
	 */
	static Vector3 Vec3(const Vector3& a, const Vector3& b)
	{
		return Vector3{ Float(a.x, b.x), Float(a.y, b.y), Float(a.z, b.z) };
	}

#pragma endregion

#pragma region Vector4

	/**
	 * \return Vector4 with each element in [0, 1]
	 */
	static Vector4 Vec4()
	{
		return Vector4{ Float(), Float(), Float(), Float() };
	}

	/**
	 * \return Vector4 with each element in [min, max]
	 */
	static Vector4 Vec4(const float min, const float max)
	{
		return Vector4{ Float(min, max), Float(min, max), Float(min, max), Float(min, max) };
	}

	/**
	 * \return Vector3 with x = [a.x, b.x], y = [a.y, b.y], z = [a.z, b.z], w = [a.w, b.w]
	 */
	static Vector4 Vec4(const Vector4& a, const Vector4& b)
	{
		return Vector4{ Float(a.x, b.x), Float(a.y, b.y), Float(a.z, b.z), Float(a.w, b.w) };
	}

#pragma endregion

#pragma region Color

	/**
	 * \return Color with each element is same and in [min, max] and a = alpha
	 */
	static LevEngine::Color SmoothColor(const float min, const float max, const float alpha = 1.0f)
	{
		const auto delta = Float();
		const auto value = Math::Lerp(min, max, delta);
		return LevEngine::Color{ value, value, value, alpha };
	}

	/**
	 * \return Color with random lerp color from a to b
	 */
	static LevEngine::Color SmoothColor(const LevEngine::Color& a, const LevEngine::Color& b)
	{
		const auto delta = Float();
		return LevEngine::Color{ Math::Lerp(a.r, b.r, delta), Math::Lerp(a.g,b.g, delta), Math::Lerp(a.b, b.b, delta), Math::Lerp(a.a, b.a, delta) };
	}

	/**
	 * \return Color with each element in [min, max] and a = alpha
	 */
	static LevEngine::Color Color(const float min, const float max, const float alpha)
	{
		return LevEngine::Color{ Float(min, max), Float(min, max), Float(min, max), alpha };
	}

	/**
	 * \return Color with r = [a.r, b.r], g = [a.g, b.g], b = [a.b, b.b], r = [a.a, b.a]
	 */
	static LevEngine::Color Color(const LevEngine::Color& a, const LevEngine::Color& b)
	{
		return LevEngine::Color{ Float(a.r, b.r), Float(a.g,b.g), Float(a.b, b.b), Float(a.a, b.a) };
	}

#pragma endregion

#pragma region Quaternion
	
	/**
	 * \return Quaternion from random yaw pitch roll
	 */
	static Quaternion Rotation()
	{
		const float x = Float(-180.0f, 180.0f);
		const float y = Float(-180.0f, 180.0f);
		const float z = Float(-180.0f, 180.0f);

		return Quaternion::CreateFromYawPitchRoll(x, y, z);
	}
	
#pragma endregion
	
private:
	using U32 = uint_least32_t;
	using Engine = std::mt19937;

	static inline Scope<Engine> generator;
};
}
