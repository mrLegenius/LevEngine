#pragma once
#include "MaterialAsset.h"
#include "Renderer/MaterialPBR.h"

namespace LevEngine
{
    class TextureAsset;

    class MaterialPBRAsset final : public MaterialAsset
    {
    public:
        Material& GetMaterial() override { return m_Material; }
        Ref<TextureAsset>& GetAlbedo() { return m_Albedo; }
        Ref<TextureAsset>& GetMetallic() { return m_Metallic; }
        Ref<TextureAsset>& GetRoughness() { return m_Roughness; }
        Ref<TextureAsset>& GetNormal() { return m_Normal; }
        Ref<TextureAsset>& GetAmbientOcclusion() { return m_AmbientOcclusion; }
        Ref<TextureAsset>& GetEmissive() { return m_Emissive; }

        explicit MaterialPBRAsset(const Path& path, const UUID uuid) : MaterialAsset(path, uuid)
        {
            ShaderAssets::ForwardPBR();
        }

    protected:
        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(YAML::Node& node) override;

    private:
        MaterialPBR m_Material;

        Ref<TextureAsset> m_Albedo;
        Ref<TextureAsset> m_Metallic;
        Ref<TextureAsset> m_Roughness;
        Ref<TextureAsset> m_Normal;
        Ref<TextureAsset> m_AmbientOcclusion;
        Ref<TextureAsset> m_Emissive;
    };
}
