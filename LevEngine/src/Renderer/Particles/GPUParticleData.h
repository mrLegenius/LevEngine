#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
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

        float LifeTime;
        float Age;
        float GravityScale;
    };
}

