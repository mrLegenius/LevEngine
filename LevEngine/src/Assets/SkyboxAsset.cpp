#include "levpch.h"
#include "SkyboxAsset.h"
#include "TextureAsset.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	const Ref<Texture>& SkyboxAsset::GetTexture() const
	{
		auto& texture = HDRTexture ? HDRTexture->GetTexture() : nullptr;
		return texture;
	}

	void SkyboxAsset::SerializeData(YAML::Emitter& out)
	{
		SerializeAsset(out, "Texture", HDRTexture);
	}

	void SkyboxAsset::DeserializeData(YAML::Node& node)
	{
		HDRTexture = DeserializeAsset<TextureAsset>(node["Texture"]);
	}
}

