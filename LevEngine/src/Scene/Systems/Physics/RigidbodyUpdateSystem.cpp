#include "levpch.h"
#include "RigidbodyUpdateSystem.h"

#include "Physics/Components/Rigidbody.h"

namespace LevEngine
{
    void RigidbodyUpdateSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<Rigidbody, Transform>();
        
        for (const auto entity : view)
        {
            auto [rigidbody, transform] = view.get<Rigidbody, Transform>(entity);
            rigidbody.SetTransformScale(transform.GetWorldScale());
        }
    }
}
