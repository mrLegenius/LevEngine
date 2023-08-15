#pragma once
#include "Assets/SkyboxAsset.h"

namespace LevEngine
{
	struct SkyboxRendererComponent
	{
		Ref<SkyboxAsset> skybox;

		SkyboxRendererComponent(const Ref<SkyboxAsset>& skybox) : skybox(skybox) { }
		SkyboxRendererComponent() = default;
		SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
	};
}
