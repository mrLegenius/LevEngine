#pragma once

namespace LevEngine
{
	enum class ShaderType
	{
		None = 0,
		Vertex = 1,
		Geometry = 2,
		Pixel = 4,
		Compute = 8,
	};

	inline ShaderType operator|(const ShaderType a, const ShaderType b)
	{
		return static_cast<ShaderType>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool operator&(const ShaderType a, const ShaderType b)
	{
		return static_cast<int>(a) & static_cast<int>(b);
	}
}
