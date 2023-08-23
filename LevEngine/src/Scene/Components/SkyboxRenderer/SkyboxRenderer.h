#pragma once
#include "Assets/SkyboxAsset.h"

namespace LevEngine
{
	struct SkyboxRendererComponent
	{
		Ref<SkyboxAsset> skybox;

		SkyboxRendererComponent();
		SkyboxRendererComponent(const Ref<SkyboxAsset>& skybox) : skybox(skybox) { }
		SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
	};
}
