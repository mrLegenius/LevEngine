#pragma once
#include "FPSGame/Components/Enemy.h"
#include "FPSGame/Components/Player.h"

namespace Sandbox
{
    using namespace LevEngine;
    class EnemyMovementSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto enemyView = registry.view<Transform, LegacyRigidbody, Enemy>();

            const auto playerView = registry.view<Transform, Player>();

            Vector3 playerPosition;
            bool foundPlayer = false;
			
            for (const auto player : playerView)
            {
                playerPosition = playerView.get<Transform>(player).GetWorldPosition();
                foundPlayer = true;
            }

            if (!foundPlayer) return;
			
            for (const auto entity : enemyView)
            {
                auto [enemyTransform, enemyRigidbody, enemy] = enemyView.get<Transform, LegacyRigidbody, Enemy>(entity);
				
                auto enemyPosition = enemyTransform.GetWorldPosition();

                auto dir = playerPosition - enemyPosition;
                dir.y = 0;
                dir.Normalize();

                enemyRigidbody.velocity = dir * enemy.Speed;
				
                enemyTransform.SetWorldRotation(Quaternion::LookRotation(dir, Vector3::Up));
            }
        }
    };
}
