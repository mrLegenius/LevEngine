#include "levpch.h"
#include "RigidbodyUpdateSystem.h"

#include "Physics/Components/Rigidbody.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void RigidbodyUpdateSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<Rigidbody, Transform>();
        
        for (const auto entity : view)
        {
            auto [rigidbody, transform] = view.get<Rigidbody, Transform>(entity);
            rigidbody.SetTransformScale(transform.GetWorldScale());

            rigidbody.ApplyForce(rigidbody.GetAppliedForce());
            rigidbody.ApplyTorque(rigidbody.GetAppliedTorque());
        }
    }
}
