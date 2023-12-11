#pragma once
#include "Renderer/Camera/SceneCamera.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(CameraComponent);
	
	struct CameraComponent
	{
		SceneCamera Camera;
		bool IsMain = true;
		bool FixedAspectRatio = false;

		CameraComponent();
		CameraComponent(const CameraComponent&) = default;
	};
}
