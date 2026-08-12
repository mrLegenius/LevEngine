#pragma once

#include "Scene/Components/Transform/Transform.h"

namespace LevEngine::Editor
{
	//<--- The cached model matrix is only recalculated by the systems, which do not run in the edit mode ---<<
	inline Matrix GetWorldMatrix(const Transform& transform)
	{
		return Matrix::CreateScale(transform.GetWorldScale()) *
			Matrix::CreateFromQuaternion(transform.GetWorldRotation()) *
			Matrix::CreateTranslation(transform.GetWorldPosition());
	}
}
