#pragma once

namespace LevEngine
{
    class Texture; 
    struct ParticlesTextureArray
    {
        static constexpr uint32_t MaxTextureSlots = 32;
        using TextureArray = Array<Ref<Texture>, MaxTextureSlots>;
    
        TextureArray TextureSlots{};
        uint32_t TextureSlotIndex{};
    };
}
