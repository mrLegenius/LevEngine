#pragma once

#include "Asset.h"
#include "Renderer/Material.h"

namespace LevEngine
{
class MaterialAsset final : public Asset
{
public:
	Material material;

	explicit MaterialAsset(const std::filesystem::path& path, const UUID uuid) : Asset(path, uuid)
	{
		Deserialize();
	}

	void DrawProperties() override;

protected:
	void SerializeData(YAML::Emitter& out) override;
	void DeserializeData(YAML::Node& node) override;

private:
	static void DrawMaterialTexture(const std::string& label, Material& material, Material::TextureType textureType);
};
}
