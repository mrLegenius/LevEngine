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
		m_Data.Ambient = Vector3{ color.R, color.G, color.B };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetAmbientColor() const
	{
		return Color{ m_Data.Ambient.x, m_Data.Ambient.y, m_Data.Ambient.z };
	}

	void SetEmissiveColor(const Color color)
	{
		m_Data.Emissive = Vector3{ color.R, color.G, color.B };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetEmissiveColor() const
	{
		return Color{ m_Data.Emissive.x, m_Data.Emissive.y, m_Data.Emissive.z };
	}

	void SetDiffuseColor(const Color color)
	{
		m_Data.Diffuse = Vector3{ color.R, color.G, color.B };
		m_IsDirty = true;
	}
	[[nodiscard]] Color GetDiffuseColor() const
	{
		return Color{ m_Data.Diffuse.x, m_Data.Diffuse.y, m_Data.Diffuse.z };
	}

	void SetSpecularColor(const Color color)
	{
		m_Data.Specular = Vector3{ color.R, color.G, color.B };
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

	void Bind(const Ref<Shader>& shader);
	void Unbind(const Ref<Shader>& shader);

private:

	struct GPUData
	{
		alignas(16) Vector3 Ambient = Vector3{ 0, 0, 0 };
		alignas(16) Vector3 Emissive = Vector3{ 0, 0, 0 };
		alignas(16) Vector3 Diffuse = Vector3{ 1,1, 1 };
		alignas(16) Vector3 Specular = Vector3{ 0, 0, 0 };

		float Shininess = 2;
		uint32_t HasAmbientTexture = false;
		uint32_t HasEmissiveTexture = false;
		uint32_t HasDiffuseTexture = false;
		uint32_t HasSpecularTexture = false;
		uint32_t HasNormalTexture = false;
	};

	using TextureMap = std::map<TextureType, Ref<Texture>>;
	TextureMap m_Textures;

	Ref<ConstantBuffer> m_ConstantBuffer;

	GPUData m_Data{};

	bool m_IsDirty;
};
}
