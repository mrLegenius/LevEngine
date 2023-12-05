#pragma once
#include "FPSGame/Components/Projectile.h"

namespace Sandbox
{
    class ProjectileLifeSystem final : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto projectileView = registry.view<Transform, Projectile>();

            for (const auto entity : projectileView)
            {
                auto [projectileTransform, projectile] = projectileView.get<Transform, Projectile>(entity);

                projectile.Timer += deltaTime;
                if (projectile.Timer >= projectile.Lifetime)
                {
                    SceneManager::GetActiveScene()->DestroyEntity(entity);
                }
            }
        }
    };
}
