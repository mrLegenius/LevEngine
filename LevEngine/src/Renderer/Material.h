#pragma once

#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "TextureLibrary.h"
#include "DataTypes/Map.h"
#include "Kernel/Color.h"

namespace LevEngine
{
class Material
{
public:
	Material();

	enum class TextureType
	{
		Emissive = 1,
		Diffuse = 2,
		Specular = 3,
		Normal = 4,
	};

	void SetAmbientColor(const Color color)
	{
		m_Data.Ambient = Vector3{ color.r, color.g, color.b };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetAmbientColor() const
	{
		return Color{ m_Data.Ambient.x, m_Data.Ambient.y, m_Data.Ambient.z };
	}

	void SetEmissiveColor(const Color color)
	{
		m_Data.Emissive = Vector3{ color.r, color.g, color.b };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetEmissiveColor() const
	{
		return Color{ m_Data.Emissive.x, m_Data.Emissive.y, m_Data.Emissive.z };
	}

	void SetDiffuseColor(const Color color)
	{
		m_Data.Diffuse = Vector3{ color.r, color.g, color.b };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetDiffuseColor() const
	{
		return Color{ m_Data.Diffuse.x, m_Data.Diffuse.y, m_Data.Diffuse.z };
	}

	void SetSpecularColor(const Color color)
	{
		m_Data.Specular = Vector3{ color.r, color.g, color.b };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetSpecularColor() const
	{
		return Color{ m_Data.Specular.x, m_Data.Specular.y, m_Data.Specular.z };
	}

	void SetShininess(const float value)
	{
		m_Data.Shininess = value;
		m_IsDirty = true;
	}
	[[nodiscard]] float GetShininess() const
	{
		return m_Data.Shininess;
	}

	void SetTexture(const TextureType type, const Ref<Texture>& texture)
	{
		m_Textures[type] = texture ? texture : TextureLibrary::GetEmptyTexture();

		m_IsDirty = true;
	}
	[[nodiscard]] Ref<Texture> GetTexture(const TextureType type) const
	{
		const auto it = m_Textures.find(type);
		return it != m_Textures.end() ? it->second : TextureLibrary::GetEmptyTexture();
	}

	void SetTextureTiling(const float value)
	{
		SetTextureTiling(Vector2{ value, value });
	}

	void SetTextureTiling(const Vector2 value)
	{
		m_Data.Tiling = value;
		m_IsDirty = true;
	}
	[[nodiscard]] Vector2 GetTextureTiling() const
	{
		return m_Data.Tiling;
	}

	void SetTextureOffset(const float value)
	{
		SetTextureOffset(Vector2{ value, value });
	}

	void SetTextureOffset(const Vector2 value)
	{
		m_Data.Offset = value;
		m_IsDirty = true;
	}
	[[nodiscard]] Vector2 GetTextureOffset() const
	{
		return m_Data.Offset;
	}

	void Bind(const Ref<Shader>& shader);
	void Unbind(const Ref<Shader>& shader);

private:
	struct alignas(16) GPUData
	{
		alignas(16) Vector3 Ambient = Vector3{ 1, 1, 1 }; //<--- 16 ---<<
		alignas(16) Vector3 Emissive = Vector3{ 0, 0, 0 }; //<--- 16 ---<<
		alignas(16) Vector3 Diffuse = Vector3{ 1,1, 1 }; //<--- 16 ---<<
		alignas(16) Vector3 Specular = Vector3{ 1, 1, 1 }; //<--- 16 ---<<

		alignas(8) Vector2 Tiling{ 1, 1 };
		alignas(8) Vector2 Offset{ 0, 0 };
		
		float Shininess = 2;
	};

	using TextureMap = Map<TextureType, Ref<Texture>>;
	TextureMap m_Textures;

	Ref<ConstantBuffer> m_ConstantBuffer;

	GPUData m_Data{};

	bool m_IsDirty{};
};
}
