#pragma once
#include <Math/Matrix.h>

namespace LevEngine
{
	struct BoneInfo
	{
		/*id is index in finalBoneMatrices*/
		int id;

		/*offset matrix transforms vertex from model space to bone space*/
		Matrix offset;
	};
}
