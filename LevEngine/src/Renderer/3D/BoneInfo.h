#pragma once

#include "Kernel/Core.h"
#include <Math/Matrix.h>

namespace LevEngine
{
	struct LEV_API BoneInfo
	{
		/* index in finalBoneMatrices */
		int id;

		/* transforms vertex from model space to bone space */
		Matrix offset;
	};
}
