#pragma once
#include <yaml-cpp/yaml.h>

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

	void Serialize() { Asset::Serialize(); }
	bool Deserialize() { return Asset::Deserialize(); }

protected:
	void Serialize(YAML::Emitter& out) override;
	bool Deserialize(YAML::Node& node) override;

private:
	static void DrawMaterialTexture(const std::string& label, Material& material, Material::TextureType textureType);
};
}
