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
            /*
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
            */

            const auto projectileView = registry.view<Transform, Projectile, Rigidbody>();
            for (auto& entity : projectileView)
            {
                auto [transform, projectile, rigidbody] = projectileView.get<Transform, Projectile, Rigidbody>(entity);

                rigidbody.OnCollisionEnter(
                    [] (const Collision& collisionInfo)
                    {
                        Log::Debug("Projectile enters {0}", collisionInfo.ContactEntity.GetName());

                        Log::Info("ADDITIONAL INFO");
                        for (int i = 0; i < collisionInfo.ContactPositions.size(); i++)
                        {
                            const auto& position = collisionInfo.ContactPositions[i];
                            Log::Info("Position: x: {0}, y: {1}, z: {2}", position.x, position.y, position.z);
                        }
                        for (int i = 0; i < collisionInfo.ContactNormals.size(); i++)
                        {
                            const auto& normal = collisionInfo.ContactNormals[i];
                            Log::Info("Normal: x: {0}, y: {1}, z: {2}", normal.x, normal.y, normal.z);
                        }
                        for (int i = 0; i < collisionInfo.ContactImpulses.size(); i++)
                        {
                            const auto& impulse = collisionInfo.ContactImpulses[i];
                            Log::Info("Impulse: x: {0}, y: {1}, z: {2}", impulse.x, impulse.y, impulse.z);
                        }
                        for (int i = 0; i < collisionInfo.ContactSeparations.size(); i++)
                        {
                            const auto& separation = collisionInfo.ContactSeparations[i];
                            Log::Info("Separation: {0}", separation);
                        }
                        for (int i = 0; i < collisionInfo.ContactFirstColliderMaterialIndices.size(); i++)
                        {
                            const auto& firstShapeMaterialIndex = collisionInfo.ContactFirstColliderMaterialIndices[i];
                            Log::Info("First Shape Material Index: {0}", firstShapeMaterialIndex);
                        }
                        for (int i = 0; i < collisionInfo.ContactSecondColliderMaterialIndices.size(); i++)
                        {
                            const auto& secondShapeMaterialIndex = collisionInfo.ContactSecondColliderMaterialIndices[i];
                            Log::Info("Second Shape Material Index: {0}", secondShapeMaterialIndex);
                        }

                        const auto scene = SceneManager::GetActiveScene();
                        if (collisionInfo.ContactEntity.HasComponent<Enemy>())
                        {
                            scene->DestroyEntity(collisionInfo.ContactEntity);
                        }
                    }
                );
            }
        }
    };
}