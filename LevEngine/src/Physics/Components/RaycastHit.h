#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
    struct RaycastHit
    {
        bool IsSuccessful = false;
        Entity Entity;
        Vector3 Point = Vector3::Zero;
        Vector3 Normal = Vector3::Zero;
        float Distance = 0.0f;
    };
}