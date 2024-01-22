#pragma once
#include <Math/Matrix.h>
#include "cereal/access.hpp"

namespace LevEngine
{
	struct BoneInfo
	{
		/* index in finalBoneMatrices */
		size_t id;

		/* transforms vertex from model space to bone space */
		Matrix offset;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(id);
			archive(offset);
		}
	};
}
