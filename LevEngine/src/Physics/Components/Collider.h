﻿#pragma once
#include "PhysicMaterial.h"

namespace LevEngine
{
    struct Collider
    {
        enum class Type
        {
            Sphere,
            Capsule,
            Box
        };

        Type m_Type = Type::Box;
        
        bool m_IsTriggerEnabled = false;
        bool m_IsContactsEnabled = false;
    
        Vector3 OffsetPosition = Vector3::Zero;
        Vector3 OffsetRotation = Vector3::Zero;

        Ref<PhysicMaterial> m_PhysicMaterial { CreateRef<PhysicMaterial>() };
    };

    struct Sphere : Collider
    {
        float Radius = 0.5f;
    };

    struct Capsule : Collider
    {
        float Radius = 0.5f;
        float HalfHeight = 0.5f;
    };

    struct Box : Collider
    {
        Vector3 HalfExtents = Vector3(0.5f, 0.5f, 0.5f);
    };
}