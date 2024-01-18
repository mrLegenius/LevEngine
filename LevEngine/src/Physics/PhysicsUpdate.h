#pragma once

namespace LevEngine
{
    class PhysicsUpdate
    {
    public:
        void ClearBuffers(entt::registry& registry);
        void UpdateTransforms(entt::registry& registry);
        void UpdateConstantForces(entt::registry& registry);
        void UpdateControllerGroundFlag(entt::registry& registry);
        void ApplyControllerGravity(entt::registry& registry, float deltaTime);
        void ApplyKinematicTargets(entt::registry& registry);
    };
}