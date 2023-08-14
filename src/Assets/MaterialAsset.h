#pragma once

#include "Asset.h"
#include "Renderer/Material.h"

namespace LevEngine
{
class MaterialAsset final : public Asset
{
public:
	Material material;

	explicit MaterialAsset(const std::filesystem::path& path) : Asset(path) { }

	void DrawProperties() override;

protected:
	void SerializeData(YAML::Emitter& out) override;
	bool DeserializeData(YAML::Node& node) override;

private:
	static void DrawMaterialTexture(const std::string& label, Material& material, Material::TextureType textureType);
};
}
