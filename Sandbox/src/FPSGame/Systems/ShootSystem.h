#pragma once
#include "Physics/Physics.h"

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
                    projectileParams.Lifetime = 2;
                    projectileParams.Timer = 0;

                    // TODO: NEED TO FIX INITIALIZATION SYSTEM
                    auto& projectileRigidbody = projectile.GetComponent<Rigidbody>();
                    projectileRigidbody.Initialize(projectileTransform);
                    const auto& gameObject = Entity(entt::handle(registry, projectile));
                    App::Get().GetPhysics().m_ActorEntityMap.insert({projectileRigidbody.GetActor(), gameObject});
                    Log::Debug("RIGIDBODY CREATED");
                    Log::Debug("ACTOR MAP SIZE: {0}", App::Get().GetPhysics().m_ActorEntityMap.size());
                    projectileRigidbody.AddForce(projectileParams.Speed * cameraTransform.GetForwardDirection(), Rigidbody::ForceMode::Impulse);
                    
                    Audio::PlayOneShot("event:/Shot", projectile);
                }
            }
        }
    };
}
