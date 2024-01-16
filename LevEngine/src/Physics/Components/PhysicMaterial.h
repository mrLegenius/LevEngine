#pragma once

struct PhysicMaterial
{
    enum class CombineMode
    {
        Average,
        Minimum,
        Multiply,
        Maximum
    };
    
    float DynamicFriction = 0.5f;
    float StaticFriction = 0.5f;
    float Bounciness = 0.6f;

    CombineMode m_FrictionCombine = CombineMode::Average;
    CombineMode m_BounceCombine = CombineMode::Average;
};