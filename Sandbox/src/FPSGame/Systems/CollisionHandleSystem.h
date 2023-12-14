#pragma once
#include "FPSGame/Components/Enemy.h"
#include "FPSGame/Components/Player.h"
#include "FPSGame/Components/Projectile.h"

namespace Sandbox
{
    using namespace LevEngine;
    class CollisionHandleSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto projectileView = registry.view<Transform, Projectile, Rigidbody>();
            for (auto entity : projectileView)
            {
                auto [projectileTransform, projectile, projectileRigidbody] = projectileView.get<Transform, Projectile, Rigidbody>(entity);
                
                projectileRigidbody.OnCollisionEnter(
                    [] (const Collision& collisionInfo)
                    {
                        Log::Debug("Object touches {0}", collisionInfo.ContactEntity.GetName());
                        
                        if (collisionInfo.ContactEntity.HasComponent<Enemy>())
                        {
                            const auto scene = SceneManager::GetActiveScene();
                            scene->DestroyEntity(collisionInfo.ContactEntity);
                            
                        }
                    }
                );
                
                projectileRigidbody.OnCollisionExit(
                    [] (const Collision& collisionInfo)
                    {
                        Log::Debug("Object was pushed off from {0}", collisionInfo.ContactEntity.GetName());

                        if (collisionInfo.ContactEntity.HasComponent<Enemy>())
                        {
                            const auto scene = SceneManager::GetActiveScene();
                            scene->DestroyEntity(collisionInfo.ContactEntity);
                        }
                    }
                );
            }
            
            const auto rigidbodyView = registry.view<Transform, Rigidbody>();
            for (auto& entity : rigidbodyView)
            {
                auto [rigidbodyTransform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

                if (rigidbody.IsTriggerEnabled())
                {
                    rigidbody.OnTriggerEnter(
                        [&] (const Entity& otherEntity)
                        {
                            const auto& trigger = Entity(entt::handle(registry, entity));
                            Log::Debug("{0} enters {1}", otherEntity.GetName(), trigger.GetName());
                            
                            if (otherEntity.HasComponent<Player>())
                            {
                                const auto scene = SceneManager::GetActiveScene();
                                scene->DestroyEntity(otherEntity);
                            }
                        }
                    );
                    
                    rigidbody.OnTriggerExit(
                        [&] (const Entity& otherEntity)
                        {
                            const auto& trigger = Entity(entt::handle(registry, entity));
                            Log::Debug("{0} leaves {1}", otherEntity.GetName(), trigger.GetName());

                            if (otherEntity.HasComponent<Player>())
                            {
                                const auto scene = SceneManager::GetActiveScene();
                                scene->DestroyEntity(otherEntity);
                            } 
                        }
                    );
                } 
            }
        }
    };
}
