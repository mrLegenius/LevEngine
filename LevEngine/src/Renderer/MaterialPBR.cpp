#include "levpch.h"
#include "MaterialPBR.h"

#include "TextureLibrary.h"

namespace LevEngine
{
MaterialPBR::MaterialPBR() : Material(sizeof GPUData)
{
    m_Textures[TextureType::Albedo] = TextureLibrary::GetEmptyTexture();
    m_Textures[TextureType::Metallic] = TextureLibrary::GetEmptyTexture();
    m_Textures[TextureType::Roughness] = TextureLibrary::GetEmptyTexture();
    m_Textures[TextureType::Normal] = TextureLibrary::GetEmptyNormalMap();
    m_Textures[TextureType::AmbientOcclusion] = TextureLibrary::GetEmptyTexture();
}

void MaterialPBR::SetTintColor(const Color color)
{
    m_Data.Tint = Vector3(color);
    m_IsDirty = true;
}

Color MaterialPBR::GetTintColor() const
{
    return Color(m_Data.Tint);
}

void MaterialPBR::SetMetallic(const float value)
{
    m_Data.Metallic = value;
    m_IsDirty = true;
}

float MaterialPBR::GetMetallic() const
{
    return m_Data.Metallic;
}

void MaterialPBR::SetRoughness(const float value)
{
    m_Data.Roughness = value;
    m_IsDirty = true;
}

float MaterialPBR::GetRoughness() const
{
    return m_Data.Roughness;
}

void MaterialPBR::SetTexture(const TextureType type, const Ref<Texture>& texture)
{
    m_Textures[type] = texture
                           ? texture
                           : type == TextureType::Normal
                           ? TextureLibrary::GetEmptyNormalMap()
                           : TextureLibrary::GetEmptyTexture();

    m_IsDirty = true;
}

Ref<Texture> MaterialPBR::GetTexture(const TextureType type) const
{
    const auto it = m_Textures.find(type);
    return it != m_Textures.end()
               ? it->second
               : type == TextureType::Normal
               ? TextureLibrary::GetEmptyNormalMap()
               : TextureLibrary::GetEmptyTexture();
}

void MaterialPBR::SetTextureTiling(const float value)
{
    SetTextureTiling(Vector2{ value, value });
}

void MaterialPBR::SetTextureTiling(const Vector2 value)
{
    m_Data.Tiling = value;
    m_IsDirty = true;
}

Vector2 MaterialPBR::GetTextureTiling() const
{
    return m_Data.Tiling;
}

void MaterialPBR::SetTextureOffset(const float value)
{
    SetTextureOffset(Vector2{ value, value });
}

void MaterialPBR::SetTextureOffset(const Vector2 value)
{
    m_Data.Offset = value;
    m_IsDirty = true;
}

Vector2 MaterialPBR::GetTextureOffset() const
{
    return m_Data.Offset;
}

void MaterialPBR::Bind(const Ref<Shader>& shader)
{
    for (auto [textureType, texture] : m_Textures)
    {
        if (texture)
            texture->Bind(static_cast<uint32_t>(textureType), shader->GetType());
    }
    
    Material::Bind(shader);
}

void MaterialPBR::Unbind(const Ref<Shader>& shader)
{
    for (auto [textureType, texture] : m_Textures)
    {
        if (texture)
            texture->Unbind(static_cast<uint32_t>(textureType), shader->GetType());
    }
    
    Material::Unbind(shader);
}
}
