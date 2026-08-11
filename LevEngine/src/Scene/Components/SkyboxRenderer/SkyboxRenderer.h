#pragma once

#include "Kernel/Core.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	class TextureAsset;
	class SkyboxAsset;
	
	REGISTER_PARSE_TYPE(SkyboxRendererComponent);
	
	struct LEV_API SkyboxRendererComponent
	{
		Ref<TextureAsset> SkyboxTexture;

		SkyboxRendererComponent();
		SkyboxRendererComponent(const Ref<TextureAsset>& skyboxTexture) : SkyboxTexture(skyboxTexture) { }
		SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
	};
}
