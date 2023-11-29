#include "levpch.h"
#include "SkyboxAsset.h"

#include "EngineAssets.h"
#include "TextureAsset.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	const Ref<Texture>& SkyboxAsset::GetTexture() const
	{
		auto& texture = HDRTexture ? HDRTexture->GetTexture() : nullptr;
		return texture;
	}

	Ref<Texture> SkyboxAsset::GetIcon() const { return Icons::Skybox(); }

	void SkyboxAsset::SerializeData(YAML::Emitter& out)
	{
		SerializeAsset(out, "Texture", HDRTexture);
	}

	void SkyboxAsset::DeserializeData(YAML::Node& node)
	{
		HDRTexture = DeserializeAsset<TextureAsset>(node["Texture"]);
	}
}

