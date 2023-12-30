#pragma once

namespace LevEngine
{
    class PhysicsUpdate
    {
    public:
        void ClearBuffers(entt::registry& registry);
        void UpdateTransforms(entt::registry& registry);
        void UpdateConstantForces(entt::registry& registry);
    };
}