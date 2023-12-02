#include "levpch.h"
#include "ForceUpdateSystem.h"

#include "Physics/Components/Rigidbody.h"

namespace LevEngine
{
    void ForceUpdateSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<Rigidbody, Force>();
        
        for (const auto entity : view)
        {
            auto [rigidbody, force] = view.get<Rigidbody, Force>(entity);
            force.ApplyForce(rigidbody);
        }
    }
}

