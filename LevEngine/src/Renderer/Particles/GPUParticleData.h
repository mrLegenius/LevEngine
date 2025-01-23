#pragma once

namespace LevEngine
{
    struct GPUParticleData
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

