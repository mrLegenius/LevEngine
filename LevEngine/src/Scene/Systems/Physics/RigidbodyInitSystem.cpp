#include "levpch.h"
#include "RigidbodyInitSystem.h"

#include "Physics/Components/Rigidbody.h"
#include "Scene/Components/Transform/Transform.h"

void LevEngine::RigidbodyInitSystem::Update(float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<Rigidbody, Transform>();

    for (const auto entity : view)
    {
        if (auto [rigidbody, transform] = view.get<Rigidbody, Transform>(entity); !rigidbody.IsInitialized())
        {
            rigidbody.Initialize(transform);
        }
    }
}