#pragma once
#include "MaterialAsset.h"
#include "Renderer/Material/MaterialSimple.h"

namespace LevEngine
{
	class TextureAsset;

	class MaterialSimpleAsset final : public MaterialAsset
{
public:
	Material& GetMaterial() override { return m_Material; }
	Ref<TextureAsset>& GetDiffuse() { return m_Diffuse; }
	Ref<TextureAsset>& GetEmissive() { return m_Emissive; }
	Ref<TextureAsset>& GetSpecular() { return m_Specular; }
	Ref<TextureAsset>& GetNormal() { return m_Normal; }
	
	explicit MaterialSimpleAsset(const Path& path, const UUID uuid) : MaterialAsset(path, uuid) { }

protected:
	void SerializeData(YAML::Emitter& out) override;
	void DeserializeData(const YAML::Node& node) override;

private:
	MaterialSimple m_Material;
	
	Ref<TextureAsset> m_Diffuse;
	Ref<TextureAsset> m_Emissive;
	Ref<TextureAsset> m_Specular;
	Ref<TextureAsset> m_Normal;
};


}
