#pragma once
#include "MaterialAsset.h"
#include "Renderer/Material/MaterialPBR.h"

namespace LevEngine
{
    class TextureAsset;

    class MaterialPBRAsset final : public MaterialAsset
    {
    public:
        Material& GetMaterial() override { return m_Material; }

        Ref<TextureAsset> GetTexture(MaterialPBR::TextureType type);
        void SetTexture(MaterialPBR::TextureType type, const Ref<TextureAsset>& textureAsset);

        explicit MaterialPBRAsset(const Path& path, UUID uuid);
        explicit MaterialPBRAsset(const Path& path, UUID uuid, const MaterialPBR& material);

    protected:
        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(const YAML::Node& node) override;

    private:
        MaterialPBR m_Material;

        UnorderedMap<MaterialPBR::TextureType, Ref<TextureAsset>> m_Textures;
    };
}
