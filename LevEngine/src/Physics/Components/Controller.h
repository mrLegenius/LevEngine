#pragma once
#include "PhysicalMaterial.h"

namespace LevEngine
{
    struct Controller
    {
        enum class ClimbingMode
        {
            Easy,
            Constrained
        };

        float Radius = 0.5f;
        float HalfHeight = 0.5f;
        ClimbingMode m_ClimbingMode = ClimbingMode::Constrained;
        
        enum class NonWalkableMode
        {
            PreventClimbing,
            PreventClimbingAndForceSliding
        };

        Vector3 Center = Vector3::Zero;
        
        float SlopeLimit = 0.707f;
        float StepOffset = 0.5f;
        float ContactOffset = 0.1f;
        float MinimumMovementDistance = 0.001f;
        NonWalkableMode m_NonWalkableMode = NonWalkableMode::PreventClimbing;

        Ref<PhysicalMaterial> m_PhysicalMaterial { CreateRef<PhysicalMaterial>() };
    };
}
