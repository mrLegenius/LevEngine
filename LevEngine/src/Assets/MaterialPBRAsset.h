#pragma once
#include "MaterialAsset.h"
#include "TextureAsset.h"
#include "Renderer/MaterialPBR.h"

namespace LevEngine
{
class MaterialPBRAsset final : public MaterialAsset
{
public:
    Material& GetMaterial() override { return m_Material; }
    explicit MaterialPBRAsset(const Path& path, const UUID uuid) : MaterialAsset(path, uuid)
    {
        ShaderAssets::ForwardPBR();
    }

    void DrawProperties() override;

protected:
    void SerializeData(YAML::Emitter& out) override;
    void DeserializeData(YAML::Node& node) override;

private:
    MaterialPBR m_Material;
	
    static void DrawMaterialTexture(const String& label, MaterialPBR& material, MaterialPBR::TextureType textureType, Ref<TextureAsset>& textureAsset);

    Ref<TextureAsset> m_Albedo;
    Ref<TextureAsset> m_Metallic;
    Ref<TextureAsset> m_Roughness;
    Ref<TextureAsset> m_Normal;
    Ref<TextureAsset> m_AmbientOcclusion;
    Ref<TextureAsset> m_Emissive;
};
}
