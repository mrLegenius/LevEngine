#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    // The material facing part of a compiled shader: the fields of its material constant buffer
    // and the textures it expects a material to bind. Filled in from reflection, so the shader
    // file is the single source of truth for what a material property is called and where it
    // goes. See MaterialCustom, which is nothing but values keyed by these names.

    enum class ShaderPropertyType
    {
        Unknown,
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Bool,
    };

    // Textures at or above this register belong to the engine (shadow map, IBL cubemaps -- see
    // Registers.hlsli), so a material never gets to own them.
    inline constexpr uint32_t k_MaterialTextureSlotCount = 9;

    inline constexpr const char* k_MaterialConstantBufferName = "MaterialConstantBuffer";

    // One scalar or vector field of the material constant buffer.
    struct ShaderProperty
    {
        String Name;
        ShaderPropertyType Type{ShaderPropertyType::Unknown};
        uint32_t Offset{}; //<--- Byte offset into the material constant buffer ---<<
    };

    // One texture a material binds, at the register the shader declared it in.
    struct ShaderTextureProperty
    {
        String Name;
        uint32_t Slot{};
    };

    struct ShaderMaterialLayout
    {
        Vector<ShaderProperty> Properties;
        Vector<ShaderTextureProperty> Textures;

        //<--- Size of the material constant buffer, 0 when the shader declares none ---<<
        uint32_t BufferSize{};

        [[nodiscard]] bool IsEmpty() const { return Properties.empty() && Textures.empty(); }

        void Clear()
        {
            Properties.clear();
            Textures.clear();
            BufferSize = 0;
        }
    };

    [[nodiscard]] inline uint32_t GetShaderPropertyComponentCount(const ShaderPropertyType type)
    {
        switch (type)
        {
        case ShaderPropertyType::Float2:
            return 2;
        case ShaderPropertyType::Float3:
            return 3;
        case ShaderPropertyType::Float4:
            return 4;
        case ShaderPropertyType::Float:
        case ShaderPropertyType::Int:
        case ShaderPropertyType::Bool:
            return 1;
        default:
            return 0;
        }
    }
}
