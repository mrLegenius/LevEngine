#pragma once

namespace Sandbox
{
    class ShootSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto cameraView = registry.view<Transform, CameraComponent>();

            for (const auto entity : cameraView)
            {
                auto [cameraTransform, camera] = cameraView.get<Transform, CameraComponent>(entity);

                if (!camera.IsMain) continue;

                if (Input::IsMouseButtonPressed(MouseButton::Left))
                {
                    const auto projectilePrefab = ResourceManager::LoadAsset<PrefabAsset>("ProjectilePrefab");
                    auto projectile = projectilePrefab->Instantiate(SceneManager::GetActiveScene());

                    // TODO: ADD CORRECT PROJECTILE SPAWN POSITION AND SHOOT DIRECTION
                    auto& projectileTransform = projectile.GetComponent<Transform>();
                    projectileTransform.SetWorldPosition(cameraTransform.GetWorldPosition() + cameraTransform.GetForwardDirection() * 2);

                    auto& projectileParams = projectile.AddComponent<Projectile>();
                    projectileParams.Speed = 25;
                    projectileParams.Lifetime = 5;
                    projectileParams.Timer = 0;

                    auto& projectileRigidbody = projectile.GetComponent<Rigidbody>();
                    projectileRigidbody.SetRigidbodyPose(projectileTransform);
                    projectileRigidbody.AddForce(projectileParams.Speed * cameraTransform.GetForwardDirection(), Rigidbody::ForceMode::Impulse);
                    
                    Audio::PlayOneShot("event:/Shot", projectile);
                }
            }

            const auto projectileView = registry.view<Transform, Projectile, Rigidbody>();

            for (const auto entity : projectileView)
            {
                auto [projectileTransform, projectile, projectileRigidbody] = projectileView.get<Transform, Projectile, Rigidbody>(entity);
                
                projectileRigidbody.OnCollisionEnter(
                    [] (const Entity& otherEntity)
                    {
                        Log::Debug("Object touches {0} object", otherEntity.GetName());
                    }
                );
            
                projectileRigidbody.OnCollisionExit(
                    [] (const Entity& otherEntity)
                    {
                        Log::Debug("Object pushes off from {0} object", otherEntity.GetName());
                    }
                );
            }

            const auto rigidbodyView = registry.view<Transform, Rigidbody>();

            for (const auto entity : rigidbodyView)
            {
                auto [rigidbodyTransform, rigidbody] = rigidbodyView.get<Transform, Rigidbody>(entity);

                if (rigidbody.IsTriggerEnabled())
                {
                    rigidbody.OnTriggerEnter(
                        [] (const Entity& otherEntity)
                            {
                                Log::Debug("Object {0} enters trigger", otherEntity.GetName());
                            }
                    );
            
                    rigidbody.OnTriggerExit(
                        [] (const Entity& otherEntity)
                        {
                            Log::Debug("Object {0} leaves trigger", otherEntity.GetName());
                        }
                    );
                }
            }
        }
    };
}
