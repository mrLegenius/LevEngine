#pragma once
#include <cstdlib>

struct Random
{
	static float Float01()
	{
		return static_cast<float>(rand()) / RAND_MAX;
	}

	static float Range(const float min, const float max)
	{
		return min + Float01() * (max - min);
	}

	static int Range(const int min, const int max)
	{
		return min + rand() % (max - min);
	}

	static Vector3 Vec3()
	{
		return Vector3{ Float01(), Float01(), Float01() };
	}

	static Vector3 Vec3(const Vector3 a, const Vector3 b)
	{
		return Vector3{ Range(a.x, b.x), Range(a.y, b.y), Range(a.z, b.z) };
	}

	static Vector3 Vec3(const float min, const float max)
	{
		return Vector3{ Range(min, max), Range(min, max), Range(min, max) };
	}

	static Vector4 Vec4(const float min, const float max)
	{
		return Vector4{ Range(min, max), Range(min, max), Range(min, max), Range(min, max) };
	}

	static Color Color(const Color& a, const Color& b)
	{
		return ::Color{ Range(a.x, b.x), Range(a.y, b.y), Range(a.z, b.z), Range(a.w, b.w) };
	}
};
