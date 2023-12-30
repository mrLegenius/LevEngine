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
                }
                else if (Input::IsKeyDown(KeyCode::S))
                {
                    movement -= Vector3(cameraTransform.GetForwardDirection().x, 0.0f, cameraTransform.GetForwardDirection().z);
                }
                if (Input::IsKeyDown(KeyCode::A))
                {
                    movement -= Vector3(cameraTransform.GetRightDirection().x, 0.0f, cameraTransform.GetRightDirection().z);
                }
                else if (Input::IsKeyDown(KeyCode::D))
                {
                    movement += Vector3(cameraTransform.GetRightDirection().x, 0.0f, cameraTransform.GetRightDirection().z);
                }
                
                if (Input::IsKeyDown(KeyCode::Space))
                {
                    movement += Vector3::Up;
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