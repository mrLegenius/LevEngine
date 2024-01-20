#pragma once
#include "Scene/Components/Transform/Transform.h"
#include "Components/Player.h"
#include "Physics/Components/CharacterController.h"

namespace Sandbox
{
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

                auto velocity = Vector3::Zero;
                if (Input::IsKeyDown(KeyCode::LeftShift))
                {
                    velocity = movementDirection * player.SprintSpeed;
                }
                else
                {
                    velocity = movementDirection * player.WalkSpeed;
                }

                if (Input::IsKeyDown(KeyCode::Space))
                {
                    playerController.Jump(player.JumpHeight, deltaTime);
                }

                const auto displacement = velocity * deltaTime;
                playerController.Move(displacement);
            }
        }
    };
}