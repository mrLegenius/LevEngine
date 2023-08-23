#pragma once
#include "Renderer/Camera/SceneCamera.h"

namespace LevEngine
{
	class CameraComponentDrawer;

	struct CameraComponent
	{
		SceneCamera camera;
		bool isMain = true;
		bool fixedAspectRatio = false;

		CameraComponent();
		CameraComponent(const CameraComponent&) = default;
	};
}
