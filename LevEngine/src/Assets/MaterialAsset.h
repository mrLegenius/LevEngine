#pragma once

#include "Asset.h"
#include "TextureAsset.h"
#include "Renderer/Material.h"

namespace LevEngine
{
class MaterialAsset final : public Asset
{
public:
	Material material;
	Ref<TextureAsset> diffuse;
	Ref<TextureAsset> emissive;
	Ref<TextureAsset> specular;
	Ref<TextureAsset> normal;

	explicit MaterialAsset(const Path& path, const UUID uuid) : Asset(path, uuid)
	{
		
	}

	[[nodiscard]] Ref<Texture> GetIcon() const override
	{
		return Icons::Material();
	}

protected:
	void SerializeData(YAML::Emitter& out) override;
	void DeserializeData(YAML::Node& node) override;
};
}
