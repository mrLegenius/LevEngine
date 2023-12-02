#pragma once
namespace Sandbox
{
    /*
    extern int score;
    
    inline void OnProjectileCollided(const Entity entity, const Entity other)
    {
        if (!other.HasComponent<Enemy>()) return;

        const auto scene = SceneManager::GetActiveScene();
        Audio::PlayOneShot("event:/ProjectileHit", entity);
        scene->DestroyEntity(entity);

        Audio::PlayOneShot("event:/EnemyDeath", other);
        scene->DestroyEntity(other);
        score++;
    }
    */
    
    class ShootSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, CameraComponent>();

            for (const auto entity : view)
            {
                auto [cameraTransform, camera] = view.get<Transform, CameraComponent>(entity);

                if (!camera.IsMain) continue;

                if (Input::IsMouseButtonPressed(MouseButton::Left))
                {
                    const auto projectilePrefab = ResourceManager::LoadAsset<PrefabAsset>("MissilePrefab");
                    auto projectile = projectilePrefab->Instantiate(SceneManager::GetActiveScene());

                    auto& projectileTransform = projectile.GetComponent<Transform>();
                    projectileTransform.SetWorldPosition(cameraTransform.GetWorldPosition() + cameraTransform.GetForwardDirection() * 2);
                    
                    auto& projectileParams = projectile.AddComponent<Projectile>();
                    projectileParams.Speed = 25;
                    projectileParams.Lifetime = 1;
                    projectileParams.Timer = 0;
                    
                    auto& projectileRigidbody = projectile.GetComponent<Rigidbody>();
                    projectileRigidbody.EnableGravity(false);
                    projectileRigidbody.SetStaticFriction(0.0f);
                    projectileRigidbody.SetDynamicFriction(0.0f);
                    projectileRigidbody.SetRestitution(0.0f);
                    projectileRigidbody.LockRotAxisX(true);
                    projectileRigidbody.LockRotAxisY(true);
                    projectileRigidbody.LockRotAxisZ(true);

                    auto& force = projectile.AddComponent<Force>();
                    force.SetForceType(Force::Type::Impulse);
                    force.SetLinearForce(projectileParams.Speed * cameraTransform.GetForwardDirection());
                    force.CompleteAction(false);

                    //Audio::PlayOneShot("event:/Shot", projectile);
                }
            }
        }
    };
}
