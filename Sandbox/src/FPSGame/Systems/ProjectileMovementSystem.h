#pragma once
namespace Sandbox
{
    class ProjectileMovementSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, Projectile>();

            for (const auto entity : view)
            {
                auto [transform, projectile] = view.get<Transform, Projectile>(entity);

                transform.Move(projectile.Speed * deltaTime * transform.GetForwardDirection());
            }
        }
    };
}
