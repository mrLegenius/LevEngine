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
                    projectileRigidbody.AddForce(projectileParams.Speed * cameraTransform.GetForwardDirection(), Rigidbody::ForceMode::Impulse);

                    Audio::PlayOneShot("event:/Shot", projectile);
                }
            }
        }
    };
}
