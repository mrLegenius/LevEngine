#pragma once
#include "Renderer/Camera/SceneCamera.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(CameraComponent);
	
	struct CameraComponent
	{
		SceneCamera camera;
		bool isMain = true;
		bool fixedAspectRatio = false;

		CameraComponent();
		CameraComponent(const CameraComponent&) = default;
	};
}
