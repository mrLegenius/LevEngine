#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	class SkyboxAsset;
	
	REGISTER_PARSE_TYPE(SkyboxRendererComponent);
	
	struct SkyboxRendererComponent
	{
		Ref<TextureAsset> SkyboxTexture;

		SkyboxRendererComponent();
		SkyboxRendererComponent(const Ref<TextureAsset>& skyboxTexture) : SkyboxTexture(skyboxTexture) { }
		SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
	};
}
