#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
    struct ControllerColliderHit
    {
        // The entity with which the collision occurred.
        Entity Entity;
        // Contact position in world space.
        Vector3 Point = Vector3::Zero;
        // Contact normal in world space.
        Vector3 Normal = Vector3::Zero;
        // Motion direction.
        Vector3 MoveDirection = Vector3::Zero;
        // Motion length.
        float MoveLength = 0.0f;
    };
}