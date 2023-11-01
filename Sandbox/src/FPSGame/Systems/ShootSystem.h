#pragma once
namespace Sandbox
{
    extern int score;

    inline void OnProjectileCollided(const Entity entity, const Entity other)
    {
        if (!other.HasComponent<Enemy>()) return;

        const auto scene = SceneManager::GetActiveScene();
        Audio::Get().PlayOneShot("event:/ProjectileHit", entity);
        scene->DestroyEntity(entity);

        Audio::Get().PlayOneShot("event:/EnemyDeath", other);
        scene->DestroyEntity(other);
        score++;
    }
    
    class ShootSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, CameraComponent>();

            for (const auto entity : view)
            {
                auto [cameraTransform, camera] = view.get<Transform, CameraComponent>(entity);

                if (!camera.isMain) continue;

                if (Input::IsMouseButtonPressed(MouseButton::Left))
                {
                    const auto projectilePrefab = ResourceManager::LoadAsset<PrefabAsset>("MissilePrefab");
                    auto projectile = projectilePrefab->Instantiate(SceneManager::GetActiveScene());
					
                    auto& transform = projectile.GetComponent<Transform>();

                    auto childTransform = cameraTransform.GetChildren()[0].GetComponent<Transform>();
                    Vector3 offset = 2.5f * childTransform.GetForwardDirection();
                    offset += 0.1f * childTransform.GetUpDirection();

                    Vector3 origin = childTransform.GetWorldPosition() + offset;
					
                    transform.SetWorldPosition(origin);
                    transform.SetWorldRotation(childTransform.GetWorldRotation());

                    auto& events = projectile.AddComponent<CollisionEvents>();
                    events.onCollisionBegin.connect<&OnProjectileCollided>();
					
                    auto projectileMeshEntity = transform.GetChildren()[0];
                    auto& meshTransform = projectileMeshEntity.GetComponent<Transform>();
                    meshTransform.SetLocalRotation(Random::Rotation());

                    auto& projectileComp = projectile.AddComponent<Projectile>();

                    projectileComp.speed = 100;
                    projectileComp.lifetime = 1;
                    projectileComp.timer = 0;

                    Audio::Get().PlayOneShot("event:/Shot", projectile);
                }
            }
        }
    };
}
