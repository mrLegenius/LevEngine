#pragma once

#include "Material.h"
#include "DataTypes/Map.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

namespace LevEngine
{
    class Texture;

    class MaterialPBR final : public Material
{
public:
    enum class TextureType
    {
        Albedo = 0,
        Metallic = 1,
        Roughness = 2,
        Normal = 3,
        AmbientOcclusion = 4,
        Emissive = 5,
    };
    
    MaterialPBR();

    void SetTintColor(Color color);
    [[nodiscard]] Color GetTintColor() const;

    void SetMetallic(float value);
    [[nodiscard]] float GetMetallic() const;

    void SetRoughness(float value);
    [[nodiscard]] float GetRoughness() const;
    
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

    void SetEnableTransparency(bool value);
    [[nodiscard]] bool GetEnableTransparency() const;
    [[nodiscard]] bool IsTransparent() override;
    
protected:

    void* GetGPUData() override { return &m_Data; }

private:
    struct alignas(16) GPUData
    {
        alignas(16) Vector3 Tint = Vector3{ 1, 1, 1 }; //<--- 16 ---<<

        //<--- 16 ---<<
        alignas(8) Vector2 Tiling{ 1, 1 };
        alignas(8) Vector2 Offset{ 0, 0 };
        //<--- 16 ---<<

        //<--- 8 ---<<
        float Metallic = 0.04f;
        float Roughness = 0.0f;
        //<--- 8 ---<<
    };
    
    Map<TextureType, Ref<Texture>> m_Textures;
    GPUData m_Data{};

    bool m_EnableTransparency{};
};
}

