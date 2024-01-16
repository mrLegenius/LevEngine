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
            
            for (const auto entity : projectileView)
            {
                auto [transform, projectile, rigidbody] = projectileView.get<Transform, Projectile, Rigidbody>(entity);
                
                const auto scene = SceneManager::GetActiveScene();

                const auto causer = Entity(entt::handle(registry, entity));
                
                for (const auto& collisionInfo : rigidbody.GetCollisionEnterBuffer())
                {
                    if (collisionInfo.ContactEntity.HasComponent<Enemy>())
                    {
                        scene->DestroyEntity(collisionInfo.ContactEntity);
                        scene->DestroyEntity(causer);
                    }
                }
            }
        }
    };
}