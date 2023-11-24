#pragma once
#include "entt/entt.hpp"
#include "..\Components\LegacyRigidbody.h"
namespace LevEngine
{
inline void UpdatePositionSystem(const float deltaTime, entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto group = registry.group<>(entt::get<Transform, LegacyRigidbody>);
    for (const auto entity : group)
    {
        auto [transform, rigidbody] = group.get<Transform, LegacyRigidbody>(entity);

        if (rigidbody.bodyType != LegacyBodyType::Dynamic || !rigidbody.enabled) continue;

        //Linear movement
        Vector3 position = transform.GetLocalPosition();
        position += rigidbody.velocity * deltaTime;
        transform.SetLocalPosition(position);

        // Linear Damping
        const float dampingFactor = 1.0f - rigidbody.damping;
        const float frameDamping = powf(dampingFactor, deltaTime);
        rigidbody.velocity *= frameDamping;

        //Angular movement
        Quaternion orientation = transform.GetLocalRotation();

        orientation += orientation * Quaternion(rigidbody.angularVelocity * deltaTime * 0.5f, 0.0f);
        orientation.Normalize();

        transform.SetLocalRotation(orientation);

        // Angular Damping
        const float angularDampingFactor = 1.0f - rigidbody.angularDamping;
        const float angularFrameDamping = powf(angularDampingFactor, deltaTime);
        rigidbody.angularVelocity *= angularFrameDamping;
    }
}
}