#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	class SkyboxAsset;
	
	REGISTER_PARSE_TYPE(SkyboxRendererComponent);
	
	struct SkyboxRendererComponent
	{
		Ref<SkyboxAsset> skybox;

		SkyboxRendererComponent();
		SkyboxRendererComponent(const Ref<SkyboxAsset>& skybox) : skybox(skybox) { }
		SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
	};
}
