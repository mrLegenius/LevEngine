#include "levpch.h"
#include "PhysicsInitSystem.h"

#include "Physics/PhysX/PhysicsRigidbody.h"

void LevEngine::PhysicsRigidbodyInitSystem::Update(float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<PhysicsRigidbody>();

    for (const auto entity : view)
    {
        PhysicsRigidbody& physicsRigidbodyComponent = view.get<PhysicsRigidbody>(entity);

        if (!physicsRigidbodyComponent.IsInitialized())
        {
            auto entityWrapped = Entity(entt::handle{ registry, entity });
            physicsRigidbodyComponent.Init(entityWrapped);
        }
    }
}
