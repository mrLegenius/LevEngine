#include "levpch.h"
#include "PhysicsRigidbodyInitSystem.h"

#include "Physics/PhysX/PhysicsRigidbody.h"

void LevEngine::PhysicsRigidbodyInitSystem::Update(float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<PhysicsRigidbody, Transform>();

    for (const auto entity : view)
    {
        auto [rigidbody, transform] = view.get<PhysicsRigidbody, Transform>(entity);
        
        if (!rigidbody.IsInitialized())
        {
            rigidbody.Init(transform);
        }
    }
}