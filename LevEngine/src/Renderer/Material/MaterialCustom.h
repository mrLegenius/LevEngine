#pragma once

#include "Kernel/Core.h"
#include "DataTypes/Array.h"
#include "Math/Math.h"

#include "Material.h"
#include "Renderer/Shader/ShaderMaterialLayout.h"

namespace LevEngine
{
    class Texture;

    // A material drawn with a shader from the project instead of one of the engine's. Its
    // editable surface is whatever that shader declared: the fields of its
    // MaterialConstantBuffer and the textures it binds in the material slot range.
    //
    // Values are kept by name and packed into the buffer at bind time, so editing the shader
    // keeps every property that still exists, drops nothing that comes back, and needs no
    // migration of the material file.
    class LEV_API MaterialCustom final : public Material
    {
    public:
        struct PropertyValue
        {
            ShaderPropertyType Type{ShaderPropertyType::Unknown};

            union
            {
                float Float[4];
                int32_t Int[4];
            };

            PropertyValue() : Float{} { }
        };

        MaterialCustom() = default;

        //<--- An empty path leaves the material without a shader: it draws nothing of its own ---<<
        void SetShaderPath(const Path& path);
        [[nodiscard]] const Path& GetShaderPath() const { return m_ShaderPath; }

        // The layout the inspector and the serializer work against: the first permutation of the
        // shader that compiles. Empty while the shader is missing or broken.
        [[nodiscard]] const ShaderMaterialLayout& GetLayout() const;

        void SetFloat(const String& name, float value);
        [[nodiscard]] float GetFloat(const String& name) const;

        void SetVector2(const String& name, Vector2 value);
        [[nodiscard]] Vector2 GetVector2(const String& name) const;

        void SetVector3(const String& name, Vector3 value);
        [[nodiscard]] Vector3 GetVector3(const String& name) const;

        void SetVector4(const String& name, Vector4 value);
        [[nodiscard]] Vector4 GetVector4(const String& name) const;

        void SetColor(const String& name, Color value);
        [[nodiscard]] Color GetColor(const String& name) const;

        void SetInt(const String& name, int32_t value);
        [[nodiscard]] int32_t GetInt(const String& name) const;

        void SetBool(const String& name, bool value);
        [[nodiscard]] bool GetBool(const String& name) const;

        void SetTexture(const String& name, const Ref<Texture>& texture);
        [[nodiscard]] Ref<Texture> GetTexture(const String& name) const;

        void SetTransparent(const bool value) { m_IsTransparent = value; }
        [[nodiscard]] bool IsTransparent() override { return m_IsTransparent; }

        [[nodiscard]] Ref<Shader> GetShader(const MaterialShaderVariant& variant) const override;

        void Bind(const Ref<Shader>& shader) override;
        void Unbind(const Ref<Shader>& shader) override;

        // Values of properties the shader no longer declares are kept around: they come back
        // as soon as it declares them again, which happens constantly while editing a shader.
        [[nodiscard]] const UnorderedMap<String, PropertyValue>& GetPropertyValues() const { return m_Values; }
        void SetPropertyValue(const String& name, const PropertyValue& value);
        [[nodiscard]] PropertyValue GetPropertyValue(const String& name) const;

    protected:
        void* GetGPUData() override { return m_GPUData.data(); }

    private:
        void PackGPUData(const ShaderMaterialLayout& layout);

        Path m_ShaderPath;

        UnorderedMap<String, PropertyValue> m_Values;
        UnorderedMap<String, Ref<Texture>> m_Textures;

        Vector<uint8_t> m_GPUData;

        //<--- Permutations are compiled on demand and shared through the ShaderLibrary ---<<
        mutable Array<Ref<Shader>, MaterialShaderVariant::Count> m_Variants;

        bool m_IsTransparent{};
    };
}
