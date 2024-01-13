#include "levpch.h"
#include "Scene/Components/Transform/Transform.h"
#include "Physics/Physics.h"
#include "Physics/Components/Rigidbody.h"
#include "RigidbodyInitSystem.h"

namespace LevEngine
{
    void RigidbodyInitSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<Transform, Rigidbody>();

        for (const auto entity : view)
        {
            auto [rigidbodyTransform, rigidbody] = view.get<Transform, Rigidbody>(entity);

            if (!rigidbody.IsInitialized())
            {
                const auto gameObject = Entity(entt::handle(registry, entity));
                rigidbody.Initialize(gameObject);
            }
        }
    }
}