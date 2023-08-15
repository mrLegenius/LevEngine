#pragma once
#include "Renderer/Camera/SceneCamera.h"

namespace LevEngine
{
	struct CameraComponent
	{
		SceneCamera camera;
		bool isMain = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}
