#pragma once

#include "Kernel/Core.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    struct LEV_API RaycastHit
    {
        bool IsSuccessful = false;
        Entity Entity;
        Vector3 Point = Vector3::Zero;
        Vector3 Normal = Vector3::Zero;
        float Distance = 0.0f;
    };
}