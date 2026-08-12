#include "levpch.h"
#include "MaterialCustom.h"

#include "TextureLibrary.h"
#include "Assets/ShaderLibrary.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    static ShaderMacros GetMacros(const MaterialShaderVariant& variant)
    {
        ShaderMacros macros;

        if (variant.Deferred) macros["LEV_DEFERRED"] = "1";
        if (variant.Instanced) macros["WITH_INSTANCING"] = "1";
        if (variant.Animated) macros["WITH_ANIMATIONS"] = "1";

        return macros;
    }

    void MaterialCustom::SetShaderPath(const Path& path)
    {
        if (m_ShaderPath == path) return;

        m_ShaderPath = path;
        m_Variants = {};

        //<--- The old buffer belongs to the old layout ---<<
        m_GPUData.clear();
        ResizeGPUData(0);
    }

    Ref<Shader> MaterialCustom::GetShader(const MaterialShaderVariant& variant) const
    {
        if (m_ShaderPath.empty()) return nullptr;

        auto& cached = m_Variants[variant.GetIndex()];

        //<--- A shader edited into a broken state stops being used until it compiles again ---<<
        if (cached && !cached->CanDrawGeometry())
            cached = nullptr;

        if (cached) return cached;

        const auto shader = ShaderLibrary::GetOrAddShader(m_ShaderPath, GetMacros(variant));

        //<--- A permutation the shader does not support is not an error: the pass falls back ---<<
        if (!shader || !shader->CanDrawGeometry()) return nullptr;

        cached = shader;

        return cached;
    }

    const ShaderMaterialLayout& MaterialCustom::GetLayout() const
    {
        static const ShaderMaterialLayout empty;

        //<--- A shader written for one technique only still has to describe its material ---<<
        if (const auto shader = GetShader(MaterialShaderVariant{}))
            return shader->GetMaterialLayout();

        if (const auto shader = GetShader(MaterialShaderVariant{true, false, false}))
            return shader->GetMaterialLayout();

        return empty;
    }

    void MaterialCustom::SetPropertyValue(const String& name, const PropertyValue& value)
    {
        m_Values[name] = value;
        m_IsDirty = true;
    }

    MaterialCustom::PropertyValue MaterialCustom::GetPropertyValue(const String& name) const
    {
        const auto it = m_Values.find(name);
        return it != m_Values.end() ? it->second : PropertyValue{};
    }

    void MaterialCustom::SetFloat(const String& name, const float value)
    {
        PropertyValue property;
        property.Type = ShaderPropertyType::Float;
        property.Float[0] = value;

        SetPropertyValue(name, property);
    }

    float MaterialCustom::GetFloat(const String& name) const { return GetPropertyValue(name).Float[0]; }

    void MaterialCustom::SetVector2(const String& name, const Vector2 value)
    {
        PropertyValue property;
        property.Type = ShaderPropertyType::Float2;
        property.Float[0] = value.x;
        property.Float[1] = value.y;

        SetPropertyValue(name, property);
    }

    Vector2 MaterialCustom::GetVector2(const String& name) const
    {
        const auto property = GetPropertyValue(name);
        return Vector2{property.Float[0], property.Float[1]};
    }

    void MaterialCustom::SetVector3(const String& name, const Vector3 value)
    {
        PropertyValue property;
        property.Type = ShaderPropertyType::Float3;
        property.Float[0] = value.x;
        property.Float[1] = value.y;
        property.Float[2] = value.z;

        SetPropertyValue(name, property);
    }

    Vector3 MaterialCustom::GetVector3(const String& name) const
    {
        const auto property = GetPropertyValue(name);
        return Vector3{property.Float[0], property.Float[1], property.Float[2]};
    }

    void MaterialCustom::SetVector4(const String& name, const Vector4 value)
    {
        PropertyValue property;
        property.Type = ShaderPropertyType::Float4;
        property.Float[0] = value.x;
        property.Float[1] = value.y;
        property.Float[2] = value.z;
        property.Float[3] = value.w;

        SetPropertyValue(name, property);
    }

    Vector4 MaterialCustom::GetVector4(const String& name) const
    {
        const auto property = GetPropertyValue(name);
        return Vector4{property.Float[0], property.Float[1], property.Float[2], property.Float[3]};
    }

    void MaterialCustom::SetColor(const String& name, const Color value)
    {
        const auto property = GetPropertyValue(name);

        //<--- A color is a float3 or a float4 in HLSL, keep whichever the shader asked for ---<<
        if (property.Type == ShaderPropertyType::Float4)
            SetVector4(name, Vector4{value});
        else
            SetVector3(name, Vector3{value});
    }

    Color MaterialCustom::GetColor(const String& name) const
    {
        const auto property = GetPropertyValue(name);

        if (property.Type == ShaderPropertyType::Float4)
            return Color{property.Float[0], property.Float[1], property.Float[2], property.Float[3]};

        return Color{property.Float[0], property.Float[1], property.Float[2]};
    }

    void MaterialCustom::SetInt(const String& name, const int32_t value)
    {
        PropertyValue property;
        property.Type = ShaderPropertyType::Int;
        property.Int[0] = value;

        SetPropertyValue(name, property);
    }

    int32_t MaterialCustom::GetInt(const String& name) const { return GetPropertyValue(name).Int[0]; }

    void MaterialCustom::SetBool(const String& name, const bool value)
    {
        PropertyValue property;
        property.Type = ShaderPropertyType::Bool;
        property.Int[0] = value ? 1 : 0;

        SetPropertyValue(name, property);
    }

    bool MaterialCustom::GetBool(const String& name) const { return GetPropertyValue(name).Int[0] != 0; }

    void MaterialCustom::SetTexture(const String& name, const Ref<Texture>& texture)
    {
        m_Textures[name] = texture;
    }

    Ref<Texture> MaterialCustom::GetTexture(const String& name) const
    {
        const auto it = m_Textures.find(name);
        return it != m_Textures.end() ? it->second : nullptr;
    }

    void MaterialCustom::PackGPUData(const ShaderMaterialLayout& layout)
    {
        LEV_PROFILE_FUNCTION();

        //<--- Whatever the shader does not get told about stays zero ---<<
        eastl::fill(m_GPUData.begin(), m_GPUData.end(), static_cast<uint8_t>(0));

        for (const auto& property : layout.Properties)
        {
            const auto it = m_Values.find(property.Name);
            if (it == m_Values.end()) continue;

            const auto componentCount = GetShaderPropertyComponentCount(property.Type);
            if (componentCount == 0) continue;

            const auto size = componentCount * sizeof(float);
            if (property.Offset + size > m_GPUData.size()) continue;

            //<--- Both sides of the union are four bytes per component, so one copy does ---<<
            memcpy(m_GPUData.data() + property.Offset, it->second.Float, size);
        }
    }

    void MaterialCustom::Bind(const Ref<Shader>& shader)
    {
        LEV_PROFILE_FUNCTION();

        //<--- Without a shader of its own this material has nothing to say about the pass one ---<<
        if (!shader || m_ShaderPath.empty()) return;

        const auto& layout = shader->GetMaterialLayout();

        if (layout.BufferSize > 0)
        {
            if (m_GPUData.size() != layout.BufferSize)
            {
                m_GPUData.resize(layout.BufferSize);
                ResizeGPUData(layout.BufferSize);
            }

            //<--- Repacked every bind: a reloaded shader can move every property ---<<
            PackGPUData(layout);
            m_ConstantBuffer->SetData(m_GPUData.data());
            m_IsDirty = false;

            auto parameter = shader->GetShaderParameterByName(k_MaterialConstantBufferName);
            if (parameter.IsValid())
            {
                parameter.Set(m_ConstantBuffer);
                parameter.Bind();
            }
        }

        for (const auto& [name, slot] : layout.Textures)
        {
            const auto it = m_Textures.find(name);
            const auto& texture = it != m_Textures.end() && it->second ? it->second : TextureLibrary::GetWhiteTexture();

            if (texture)
                texture->Bind(slot, shader->GetType());
        }
    }

    void MaterialCustom::Unbind(const Ref<Shader>& shader)
    {
        if (!shader || m_ShaderPath.empty()) return;

        const auto& layout = shader->GetMaterialLayout();

        for (const auto& [name, slot] : layout.Textures)
        {
            const auto it = m_Textures.find(name);
            const auto& texture = it != m_Textures.end() && it->second ? it->second : TextureLibrary::GetWhiteTexture();

            if (texture)
                texture->Unbind(slot, shader->GetType());
        }

        if (layout.BufferSize == 0) return;

        const auto parameter = shader->GetShaderParameterByName(k_MaterialConstantBufferName);
        if (parameter.IsValid())
            parameter.Unbind();
    }
}
