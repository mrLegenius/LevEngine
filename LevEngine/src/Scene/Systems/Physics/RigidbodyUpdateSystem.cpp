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
            
            if (rigidbody.IsInitialized())
            {
                const Vector3 transformScale = transform.GetWorldScale();
                if (m_TransformScale != transformScale)
                {
                    rigidbody.SetTransformScale(transformScale);
                    rigidbody.ApplyTransformScale();
                    m_TransformScale = transformScale;
                }
            }
            else
            {
                if (m_TransformScale == transform.GetWorldScale()) return;
                m_TransformScale = transform.GetWorldScale();
            }
        }
    }
}
