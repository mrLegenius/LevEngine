#pragma once
#include "PhysicalMaterial.h"

namespace LevEngine
{
    struct Controller
    {
        enum class Type
        {
            BoxController,
            CapsuleController
        };

        enum class NonWalkableMode
        {
            PreventClimbing,
            PreventClimbingAndForceSliding
        };

        Type m_Type = Type::CapsuleController;
        Ref<PhysicalMaterial> m_PhysicalMaterial { CreateRef<PhysicalMaterial>() };

        Vector3 Center = Vector3::Zero;
        
        float SlopeLimit = 0.707f;
        float StepOffset = 0.5f;
        float ContactOffset = 0.1f;
        float MinimumMovementDistance = 0.001f;
        NonWalkableMode m_NonWalkableMode = NonWalkableMode::PreventClimbing;
    };

    struct BoxController : Controller // axis-aligned bounding box (AABB)
    {
        float HalfHeight = 0.5f;
        float HalfSideExtent = 0.5f;
        float HalfForwardExtent = 0.5f;
    };

    struct CapsuleController : Controller
    {
        enum class ClimbingMode
        {
            Easy,
            Constrained
        };
        
        float Radius = 0.5f;
        float HalfHeight = 0.5f;
        ClimbingMode m_ClimbingMode = ClimbingMode::Constrained;
    };
}
