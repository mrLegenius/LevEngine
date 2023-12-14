#include "levpch.h"
#include "RigidbodyInitSystem.h"

#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "Physics/Components/Rigidbody.h"

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
                rigidbody.Initialize(rigidbodyTransform);

                const auto& gameObject = Entity(entt::handle(registry, entity));
                App::Get().GetPhysics().m_ActorEntityMap.insert({rigidbody.GetActor(), gameObject});
                
                //Log::Debug("RIGIDBODY CREATED");
                //Log::Debug("ACTOR MAP SIZE: {0}", App::Get().GetPhysics().m_ActorEntityMap.size());
            }
        }
    }
}