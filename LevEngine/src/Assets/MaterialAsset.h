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

	explicit MaterialAsset(const Path& path, const UUID uuid) : Asset(path, uuid)
	{
		Deserialize();
	}

	void DrawProperties() override;

	Ref<Texture> GetIcon() const override
	{
		return Icons::Material();
	}

protected:
	void SerializeData(YAML::Emitter& out) override;
	void DeserializeData(YAML::Node& node) override;

private:
	static void DrawMaterialTexture(const String& label, Material& material, Material::TextureType textureType, Ref<TextureAsset>& textureAsset);

	Ref<TextureAsset> m_Diffuse;
	Ref<TextureAsset> m_Emissive;
	Ref<TextureAsset> m_Specular;
	Ref<TextureAsset> m_Normal;
};
}
