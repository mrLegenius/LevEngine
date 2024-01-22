#pragma once
#include <Math/Matrix.h>

namespace LevEngine
{
	struct BoneInfo
	{
		/* index in finalBoneMatrices */
		size_t id;

		/* transforms vertex from model space to bone space */
		Matrix offset;
	};
}
