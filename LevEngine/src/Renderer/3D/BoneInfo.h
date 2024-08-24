#pragma once
#include <Math/Matrix.h>

namespace LevEngine
{
	struct BoneInfo
	{
		/* index in finalBoneMatrices */
		int id;

		/* transforms vertex from model space to bone space */
		Matrix offset;
	};
}
