#pragma once
#include "Components/Projectile.h"
#include "Physics/Components/Rigidbody.h"
#include "Components/Enemy.h"

namespace Sandbox
{
    using namespace LevEngine;
    class CollisionHandleSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto projectileView = registry.view<Projectile, Rigidbody>();
            
            for (const auto entity : projectileView)
            {
                auto [projectile, rigidbody] = projectileView.get<Projectile, Rigidbody>(entity);
                
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