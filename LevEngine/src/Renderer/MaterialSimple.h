#pragma once

#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "DataTypes/Map.h"
#include "Math/Color.h"

namespace LevEngine
{
class MaterialSimple final : public Material
{
public:
	enum class TextureType
	{
		Emissive = 1,
		Diffuse = 2,
		Specular = 3,
		Normal = 4,
	};

	MaterialSimple();

	void SetAmbientColor(Color color);
	[[nodiscard]] Color GetAmbientColor() const;

	void SetEmissiveColor(Color color);
	[[nodiscard]] Color GetEmissiveColor() const;

	void SetDiffuseColor(Color color);
	[[nodiscard]] Color GetDiffuseColor() const;

	void SetSpecularColor(Color color);
	[[nodiscard]] Color GetSpecularColor() const;

	void SetShininess(float value);
	[[nodiscard]] float GetShininess() const;

	void SetTexture(TextureType type, const Ref<Texture>& texture);
	[[nodiscard]] Ref<Texture> GetTexture(TextureType type) const;

	void SetTextureTiling(float value);
	void SetTextureTiling(Vector2 value);
	[[nodiscard]] Vector2 GetTextureTiling() const;

	void SetTextureOffset(float value);
	void SetTextureOffset(Vector2 value);
	[[nodiscard]] Vector2 GetTextureOffset() const;

	void Bind(const Ref<Shader>& shader) override;
	void Unbind(const Ref<Shader>& shader) override;

protected:
	void* GetGPUData() override { return &m_Data; }

public:
	[[nodiscard]] bool IsTransparent() override { return false; }

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

	Map<TextureType, Ref<Texture>> m_Textures;
	GPUData m_Data{};
};
}
