#pragma once
#include "PhysicMaterial.h"

namespace LevEngine
{
    struct Controller
    {
        enum class ClimbingMode
        {
            Easy,
            Constrained
        };

        enum class NonWalkableMode
        {
            PreventClimbing,
            PreventClimbingAndForceSliding
        };
        
        float SlopeLimit = 45.0f;
        float StepOffset = 0.5f;
        float SkinWidth = 0.1f;
        float MinMoveDistance = 0.001f;
        NonWalkableMode m_NonWalkableMode = NonWalkableMode::PreventClimbing;
        
        Vector3 CenterOffset = Vector3::Zero;
        float Radius = 0.5f;
        float Height = 0.5f;
        ClimbingMode m_ClimbingMode = ClimbingMode::Constrained;

        Ref<PhysicMaterial> m_PhysicalMaterial { CreateRef<PhysicMaterial>() };
    };
}