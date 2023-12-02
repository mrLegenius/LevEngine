#pragma once
#include "FPSGame/Components/Projectile.h"

namespace Sandbox
{
    class ProjectileLifeSystem final : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, Projectile>();

            for (const auto entity : view)
            {
                auto [transform, projectile] = view.get<Transform, Projectile>(entity);

                projectile.Timer += deltaTime;
                if (projectile.Timer >= projectile.Lifetime)
                {
                    SceneManager::GetActiveScene()->DestroyEntity(entity);
                }
            }
        }
    };
}
