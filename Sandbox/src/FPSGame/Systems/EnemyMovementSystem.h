#pragma once
#include "FPSGame/Components/Player.h"
#include "Physics/Physics.h"

namespace Sandbox
{
    class EnemyMovementSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto enemyView = registry.view<Transform, Enemy, CharacterController>();
            
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
                auto [transform, enemy, controller] = enemyView.get<Transform, Enemy, CharacterController>(entity);

                auto enemyPosition = transform.GetWorldPosition();

                const auto gravity = App::Get().GetPhysics().GetGravity();

                auto direction = playerPosition - enemyPosition;
                direction.y = 0.0f;
                direction.Normalize();

                transform.SetWorldRotation(Quaternion::LookRotation(direction, Vector3::Up));

                auto displacement = direction * enemy.Speed * 0.1f;

                // apply gravity
                if (displacement.y <= 0.0f)
                {
                    displacement.y = gravity.y * deltaTime;
                }
                else
                {
                    displacement.y += gravity.y * 10.0f * deltaTime;
                }
                
                controller.Move(displacement, deltaTime);
            }
        }
    };
}