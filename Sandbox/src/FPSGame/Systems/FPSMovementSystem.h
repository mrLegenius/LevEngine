#pragma once
#include "FPSGame/Components/Player.h"

constexpr auto WALK_SPEED = 15.0f;
constexpr auto SPRINT_SPEED = 30.0f;
constexpr auto JUMP_FORCE = 50.0f;

namespace Sandbox
{
    class FPSMovementSystem final : public System
    {
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto playerView = registry.view<Transform, Player, Rigidbody>();
            
            for (const auto entity : playerView)
            {
                auto [playerTransform, player, playerRigidbody] = playerView.get<Transform, Player, Rigidbody>(entity);
                
                const auto& cameraTransform = playerTransform.GetChildren()[0].GetComponent<Transform>();
                
                auto movement = Vector3::Zero;

                if (Input::IsKeyDown(KeyCode::LeftShift))
                {
                    player.Speed = SPRINT_SPEED;
                }
                else
                {
                    player.Speed = WALK_SPEED;
                }
                
                if (Input::IsKeyDown(KeyCode::W))
                {
                    movement += Vector3(cameraTransform.GetForwardDirection().x, 0.0f, cameraTransform.GetForwardDirection().z);
                    //playerRigidbody.SetKinematicTargetPosition(playerTransform.GetWorldPosition() + Vector3::Forward * deltaTime * player.Speed);
                }
                else if (Input::IsKeyDown(KeyCode::S))
                {
                    movement -= Vector3(cameraTransform.GetForwardDirection().x, 0.0f, cameraTransform.GetForwardDirection().z);
                    //playerRigidbody.SetKinematicTargetPosition(playerTransform.GetWorldPosition() + Vector3::Backward * deltaTime * player.Speed);
                }
                if (Input::IsKeyDown(KeyCode::A))
                {
                    movement -= Vector3(cameraTransform.GetRightDirection().x, 0.0f, cameraTransform.GetRightDirection().z);
                    //playerRigidbody.SetKinematicTargetPosition(playerTransform.GetWorldPosition() + Vector3::Left * deltaTime * player.Speed);
                }
                else if (Input::IsKeyDown(KeyCode::D))
                {
                    movement += Vector3(cameraTransform.GetRightDirection().x, 0.0f, cameraTransform.GetRightDirection().z);
                    //playerRigidbody.SetKinematicTargetPosition(playerTransform.GetWorldPosition() + Vector3::Right * deltaTime * player.Speed);
                }
                
                if (Input::IsKeyDown(KeyCode::Space))
                {
                    movement += Vector3::Up;
                    //playerRigidbody.SetKinematicTargetPosition(playerTransform.GetWorldPosition() + Vector3::Up * deltaTime * 10);
                }
                
                movement.Normalize();
                
                playerRigidbody.AddForce(
                    Vector3(
                        deltaTime * player.Speed * movement.x,
                        deltaTime * JUMP_FORCE * movement.y,
                        deltaTime * player.Speed * movement.z
                    ),
                    Rigidbody::ForceMode::Impulse
                );
            }
        }
    };
}