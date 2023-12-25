#pragma once
#include "characterkinematic/PxController.h"
#include "Scene/Entity.h"

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

        [[nodiscard]] bool IsInitialized() const;
        void Initialize(Entity entity);

        void AttachController(Entity entity);
        
        bool m_IsInitialized = false;
        
        physx::PxController* m_Controller = NULL;

        Vector3 Position = Vector3::Zero;
        Vector3 UpDirection = Vector3::Up;
        
        float Density = 10.0f; // wtf
        
        float SlopeLimit = 0.707f; // maximum slope angle
        float StepOffset = 0.5f; // step height
        float MinimumMovementDistance = 0.001f; // minimum movement distance
        float MaximumSpeed = 100.0f; // maximum speed
        NonWalkableMode NonWalkableMode = NonWalkableMode::PreventClimbing; // slope behavior
        float ContactOffset = 0.1f; // contact offset
        float ScaleCoefficient = 0.8f; // scale
        Type m_Type = Type::CapsuleController; // shape

        Ref<Controller> m_ControllerShape { CreateRef<BoxController>() };
    };

    struct BoxController : Controller
    {
        float HalfHeight = 1.0f;
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
        float Height = 1.0f;
        ClimbingMode m_ClimbingMode = ClimbingMode::Easy;
    };
}