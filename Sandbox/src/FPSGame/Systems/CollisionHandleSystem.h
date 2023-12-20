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
                        /*
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
                        */
                        
                        scene->DestroyEntity(collisionInfo.ContactEntity);
                        scene->DestroyEntity(causer);
                    }
                }
            }
        }
    };
}