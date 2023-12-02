#include "levpch.h"
#include "RigidbodyUpdateSystem.h"

#include "Physics/Components/Rigidbody.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void RigidbodyUpdateSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<Transform, Rigidbody>();
        
        for (const auto entity : view)
        {
            auto [transform, rigidbody] = view.get<Transform, Rigidbody>(entity);
            rigidbody.SetTransformScale(transform.GetWorldScale());
        }
    }
}