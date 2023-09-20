#pragma once
#include "Asset.h"
#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "Renderer/MaterialSimple.h"

namespace LevEngine
{
class MaterialSimpleAsset final : public MaterialAsset
{
public:
	Material& GetMaterial() override { return m_Material; }
	
	explicit MaterialSimpleAsset(const Path& path, const UUID uuid) : MaterialAsset(path, uuid) { }

	void DrawProperties() override;

protected:
	void SerializeData(YAML::Emitter& out) override;
	void DeserializeData(YAML::Node& node) override;

private:
	static void DrawMaterialTexture(const String& label, MaterialSimple& material, MaterialSimple::TextureType textureType, Ref<TextureAsset>& textureAsset);
	
	MaterialSimple m_Material;
	
	Ref<TextureAsset> m_Diffuse;
	Ref<TextureAsset> m_Emissive;
	Ref<TextureAsset> m_Specular;
	Ref<TextureAsset> m_Normal;
};


}
