#pragma once

#include "Kernel/Core.h"
#include "MaterialAsset.h"
#include "Renderer/Material/MaterialCustom.h"

namespace LevEngine
{
    class ShaderAsset;
    class TextureAsset;

    // A material built on a shader from the project. Everything it stores is keyed by the names
    // the shader declared, so the file survives the shader being edited: properties that are
    // gone are ignored, properties that come back keep the value they had.
    class LEV_API MaterialCustomAsset final : public MaterialAsset
    {
    public:
        explicit MaterialCustomAsset(const Path& path, const UUID uuid) : MaterialAsset(path, uuid) { }

        [[nodiscard]] Material& GetMaterial() override { return m_Material; }
        [[nodiscard]] MaterialCustom& GetCustomMaterial() { return m_Material; }

        [[nodiscard]] const Ref<ShaderAsset>& GetShaderAsset() const { return m_Shader; }
        void SetShaderAsset(const Ref<ShaderAsset>& shader);

        [[nodiscard]] Ref<TextureAsset> GetTexture(const String& name) const;
        void SetTexture(const String& name, const Ref<TextureAsset>& texture);

    protected:
        void SerializeData(YAML::Emitter& out) override;
        void DeserializeData(const YAML::Node& node) override;

    private:
        MaterialCustom m_Material;

        Ref<ShaderAsset> m_Shader;
        UnorderedMap<String, Ref<TextureAsset>> m_Textures;
    };
}
