#include "levpch.h"
#include "MaterialSimple.h"

#include "TextureLibrary.h"

namespace LevEngine
{
MaterialSimple::MaterialSimple() : Material(sizeof GPUData)
{
	m_Textures[TextureType::Emissive] = TextureLibrary::GetWhiteTexture();
	m_Textures[TextureType::Diffuse] = TextureLibrary::GetWhiteTexture();
	m_Textures[TextureType::Specular] = TextureLibrary::GetWhiteTexture();
	m_Textures[TextureType::Normal] = TextureLibrary::GetEmptyNormalMap();
}

void MaterialSimple::SetAmbientColor(const Color color)
{
	m_Data.Ambient = Vector3(color);
	m_IsDirty = true;
}

Color MaterialSimple::GetAmbientColor() const
{
	return Color{m_Data.Ambient};
}

void MaterialSimple::SetEmissiveColor(const Color color)
{
	m_Data.Emissive = Vector3(color);
	m_IsDirty = true;
}

Color MaterialSimple::GetEmissiveColor() const
{
	return Color{m_Data.Emissive};
}

void MaterialSimple::SetDiffuseColor(const Color color)
{
	m_Data.Diffuse = Vector3(color);
	m_IsDirty = true;
}

Color MaterialSimple::GetDiffuseColor() const
{
	return Color{m_Data.Diffuse};
}

void MaterialSimple::SetSpecularColor(const Color color)
{
	m_Data.Specular = Vector3(color);
	m_IsDirty = true;
}

Color MaterialSimple::GetSpecularColor() const
{
	return Color{m_Data.Specular};
}

void MaterialSimple::SetShininess(const float value)
{
	m_Data.Shininess = value;
	m_IsDirty = true;
}

float MaterialSimple::GetShininess() const
{
	return m_Data.Shininess;
}

void MaterialSimple::SetTexture(const TextureType type, const Ref<Texture>& texture)
{
	m_Textures[type] = texture
		                   ? texture
		                   : type == TextureType::Normal
		                   ? TextureLibrary::GetEmptyNormalMap()
		                   : TextureLibrary::GetWhiteTexture();

	m_IsDirty = true;
}

Ref<Texture> MaterialSimple::GetTexture(const TextureType type) const
{
	const auto it = m_Textures.find(type);
	return it != m_Textures.end()
		       ? it->second
		       : type == TextureType::Normal
		       ? TextureLibrary::GetEmptyNormalMap()
		       : TextureLibrary::GetWhiteTexture();
}

void MaterialSimple::SetTextureTiling(const float value)
{
	SetTextureTiling(Vector2{ value, value });
}

void MaterialSimple::SetTextureTiling(const Vector2 value)
{
	m_Data.Tiling = value;
	m_IsDirty = true;
}

Vector2 MaterialSimple::GetTextureTiling() const
{
	return m_Data.Tiling;
}

void MaterialSimple::SetTextureOffset(const float value)
{
	SetTextureOffset(Vector2{ value, value });
}

void MaterialSimple::SetTextureOffset(const Vector2 value)
{
	m_Data.Offset = value;
	m_IsDirty = true;
}

Vector2 MaterialSimple::GetTextureOffset() const
{
	return m_Data.Offset;
}

void MaterialSimple::Bind(const Ref<Shader>& shader)
{
	for (auto [textureType, texture] : m_Textures)
	{
		if (texture)
			texture->Bind(static_cast<uint32_t>(textureType), shader->GetType());
	}

	Material::Bind(shader);
}

void MaterialSimple::Unbind(const Ref<Shader>& shader)
{
	for (auto [textureType, texture] : m_Textures)
	{
		if (texture)
			texture->Unbind(static_cast<uint32_t>(textureType), shader->GetType());
	}

	Material::Unbind(shader);
}
}
