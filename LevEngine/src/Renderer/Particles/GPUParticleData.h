#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    // Mirrors 'struct Particle' in LevResources/Shaders/Particles/ParticlesCommon.hlsl.
    // Structured buffers are tightly packed, so field order here is the byte layout the
    // compute shaders read -- keep the two in the same order.
    struct LEV_API GPUParticleData
    {
        Vector3 Position;
        Vector3 Velocity;

        Color StartColor;
        Color EndColor;
        Color Color;

        float StartSize;
        float EndSize;
        float Size;

        float Age;
        float LifeTime;
        float GravityScale;
    };

    // Mirrors 'struct SortedElement'. One entry per particle slot: which particle, and the
    // depth key the bitonic sort orders on.
    struct LEV_API SortedParticleData
    {
        uint32_t Index;
        float Depth;
    };
}
