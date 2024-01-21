#pragma once
#include "Scene/Components/Transform/Transform.h"
#include "FPSGame/Components/Player.h"
#include "FPSGame/Components/Enemy.h"
#include "Physics/Components/CharacterController.h"

namespace Sandbox
{
    class EnemyMovementSystem final : public System
    {
        void Update(float deltaTime, entt::registry& registry) override
        {
            Vector3 playerPosition;
            bool isPlayerFounded = false;

            const auto playerView = registry.view<Transform, Player>();
            for (const auto player : playerView)
            {
                playerPosition = playerView.get<Transform>(player).GetWorldPosition();
                isPlayerFounded = true;
            }
            
            if (!isPlayerFounded) return;

            const auto enemyView = registry.view<Transform, Enemy, CharacterController>();
            for (const auto entity : enemyView)
            {
                auto [transform, enemy, controller] = enemyView.get<Transform, Enemy, CharacterController>(entity);

                auto enemyPosition = transform.GetWorldPosition();
                auto movementDirection = playerPosition - enemyPosition;
                movementDirection.y = 0.0f;
                movementDirection.Normalize();

                transform.SetWorldRotation(
                    Quaternion::LookRotation(movementDirection, Vector3::Up)
                );

                const auto displacement = movementDirection * enemy.Speed * deltaTime;
                controller.Move(displacement);
            }
        }
    };
}