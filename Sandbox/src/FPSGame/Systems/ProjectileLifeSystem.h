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

                projectile.timer += deltaTime;
				
                if (projectile.timer >= projectile.lifetime)
                    SceneManager::GetActiveScene()->DestroyEntity(entity);
            }
        }
    };
}
