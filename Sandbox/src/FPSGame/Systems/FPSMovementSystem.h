#pragma once
#include "Scene/Components/Transform/Transform.h"
#include "Components/Player.h"
#include "Physics/Components/CharacterController.h"

namespace Sandbox
{
    constexpr float WALK_SPEED = 1.0f;
    constexpr float SPRINT_SPEED = 2.0f;
    constexpr float MOVEMENT_MULTIPLIER = 0.25f;
    
    class FPSMovementSystem final : public System
    {
        void Update(const float deltaTime, entt::registry& registry) override
        {
            constexpr auto sensitivity = 45.0f;
            const Vector2 input = Input::GetMouseDelta();
            const auto delta = input * sensitivity * deltaTime;

            const auto playerView = registry.view<Transform, Player, CharacterController>();
            for (const auto playerEntity : playerView)
            {
                auto [playerTransform, player, playerController] = playerView.get<Transform, Player, CharacterController>(playerEntity);

                auto playerRotation = playerTransform.GetWorldRotation().ToEuler() * Math::RadToDeg;
                playerRotation.y -= delta.x;
                playerTransform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(playerRotation * Math::DegToRad));
                
                auto movementDirection = Vector3::Zero;
                if (Input::IsKeyDown(KeyCode::W))
                {
                    movementDirection += playerTransform.GetForwardDirection();
                }
                if (Input::IsKeyDown(KeyCode::S))
                {
                    movementDirection -= playerTransform.GetForwardDirection();
                }
                if (Input::IsKeyDown(KeyCode::A))
                {
                    movementDirection -= playerTransform.GetRightDirection();
                }
                if (Input::IsKeyDown(KeyCode::D))
                {
                    movementDirection += playerTransform.GetRightDirection();
                }
                movementDirection.Normalize();

                auto movement = movementDirection * MOVEMENT_MULTIPLIER;
                if (Input::IsKeyDown(KeyCode::LeftShift))
                {
                    movement *= SPRINT_SPEED;
                }
                else
                {
                    movement *= WALK_SPEED;
                }
                
                playerController.Move(movement, deltaTime);
            }
        }
    };
}