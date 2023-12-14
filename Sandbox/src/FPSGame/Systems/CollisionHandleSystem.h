#pragma once
#include "FPSGame/Components/Enemy.h"
#include "FPSGame/Components/Projectile.h"

namespace Sandbox
{
    using namespace LevEngine;
    class CollisionHandleSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto projectileView = registry.view<Transform, Projectile, Rigidbody>();
            for (auto& entity : projectileView)
            {
                auto [transform, projectile, rigidbody] = projectileView.get<Transform, Projectile, Rigidbody>(entity);

                if (rigidbody.IsTriggerEnabled())
                {
                    rigidbody.OnTriggerEnter(
                        [&] (const Entity& otherEntity)
                        {
                            const auto& trigger = Entity(entt::handle(registry, entity));
                            Log::Debug("{0} enters {1}", otherEntity.GetName(), trigger.GetName());

                            const auto scene = SceneManager::GetActiveScene();
                            if (otherEntity.HasComponent<Enemy>())
                            {
                                scene->DestroyEntity(otherEntity);
                            }
                            // TODO: NEED NORMAL GAME OBJECT GETTER
                            //scene->DestroyEntity(trigger);
                        }
                    );
                } 
            }
        }
    };
}
