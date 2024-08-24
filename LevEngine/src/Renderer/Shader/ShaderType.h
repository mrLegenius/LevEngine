#pragma once
#include "Essentials/EnumUtils.h"

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

	ENUM_AS_FLAG(ShaderType);
}
