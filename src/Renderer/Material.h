#pragma once
#include <map>

#include "ConstantBuffer.h"
#include "Texture.h"
#include "Kernel/Color.h"

namespace LevEngine
{
class Material
{
public:
	Material();

	enum class TextureType
	{
		Ambient = 0,
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
		m_Textures[type] = texture;
		switch (type)
		{
		case TextureType::Ambient:
			m_Data.HasAmbientTexture = texture != nullptr;
			break;
		case TextureType::Emissive:
			m_Data.HasEmissiveTexture = texture != nullptr;
			break;
		case TextureType::Diffuse:
			m_Data.HasDiffuseTexture = texture != nullptr;
			break;
		case TextureType::Specular:
			m_Data.HasSpecularTexture = texture != nullptr;
			break;
		case TextureType::Normal:
			m_Data.HasNormalTexture = texture != nullptr;
			break;
		}

		m_IsDirty = true;
	}
	[[nodiscard]] Ref<Texture> GetTexture(const TextureType type) const
	{
		const auto it = m_Textures.find(type);
		return it != m_Textures.end() ? it->second : nullptr;
	}

	void SetTextureTiling(const TextureType type, const float value)
	{
		SetTextureTiling(type, Vector2{ value, value });
	}

	void SetTextureTiling(const TextureType type, const Vector2 value)
	{
		auto& properties = GetTexturePropertiesBy(type);
		properties.Tiling = value;

		m_IsDirty = true;
	}
	[[nodiscard]] Vector2 GetTextureTiling(const TextureType type)
	{
		const auto& properties = GetTexturePropertiesBy(type);
		return properties.Tiling;
	}

	void SetTextureOffset(const TextureType type, const float value)
	{
		SetTextureOffset(type, Vector2{ value, value });
	}

	void SetTextureOffset(const TextureType type, const Vector2 value)
	{
		auto& properties = GetTexturePropertiesBy(type);
		properties.Offset = value;

		m_IsDirty = true;
	}
	[[nodiscard]] Vector2 GetTextureOffset(const TextureType type)
	{
		const auto& properties = GetTexturePropertiesBy(type);
		return properties.Offset;
	}

	void Bind(const Ref<Shader>& shader);
	void Unbind(const Ref<Shader>& shader);

private:
	struct alignas(16) TextureProperties
	{
		Vector2 Tiling{ 1, 1 };
		Vector2 Offset{ 0, 0 };
	};

	struct alignas(16) GPUData
	{
		alignas(16) Vector3 Ambient = Vector3{ 0, 0, 0 }; //<--- 16 ---<<
		alignas(16) Vector3 Emissive = Vector3{ 0, 0, 0 }; //<--- 16 ---<<
		alignas(16) Vector3 Diffuse = Vector3{ 1,1, 1 }; //<--- 16 ---<<
		alignas(16) Vector3 Specular = Vector3{ 0, 0, 0 }; //<--- 16 ---<<

		TextureProperties AmbientProperties; //<--- 16 ---<<
		TextureProperties EmissiveProperties; //<--- 16 ---<<
		TextureProperties DiffuseProperties; //<--- 16 ---<<
		TextureProperties SpecularProperties; //<--- 16 ---<<
		TextureProperties NormalProperties; //<--- 16 ---<<

		uint32_t HasAmbientTexture = false;
		uint32_t HasEmissiveTexture = false;
		uint32_t HasDiffuseTexture = false;
		uint32_t HasSpecularTexture = false;
		uint32_t HasNormalTexture = false;
		float Shininess = 2;
	};

	TextureProperties& GetTexturePropertiesBy(const TextureType type)
	{
		switch (type)
		{
		case TextureType::Ambient:
			return m_Data.AmbientProperties;
		case TextureType::Emissive:
			return m_Data.EmissiveProperties;
		case TextureType::Diffuse:
			return m_Data.DiffuseProperties;
		case TextureType::Specular:
			return m_Data.SpecularProperties;
		case TextureType::Normal:
			return m_Data.NormalProperties;
		default:
			LEV_THROW("Unknown texture type of material")
		}
	}


	using TextureMap = std::map<TextureType, Ref<Texture>>;
	TextureMap m_Textures;

	Ref<ConstantBuffer> m_ConstantBuffer;

	GPUData m_Data{};

	bool m_IsDirty;
};
}
