#include "levpch.h"
#include "RigidbodyInitSystem.h"

#include "Physics/Components/Rigidbody.h"
#include "Scene/Components/Transform/Transform.h"

void LevEngine::RigidbodyInitSystem::Update(float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<Transform, Rigidbody>();

    for (const auto entity : view)
    {
        auto [rigidbodyTransform, rigidbody] = view.get<Transform, Rigidbody>(entity);
        
        if (!rigidbody.IsInitialized())
        {
            rigidbody.Initialize(rigidbodyTransform);
        }
    }
}