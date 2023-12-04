#pragma once

constexpr auto WALK_SPEED = 15.0f;
constexpr auto SPRINT_SPEED = 30.0f;
constexpr auto JUMP_FORCE = 50.0f;

namespace Sandbox
{
    class FPSMovementSystem final : public System
    {
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto playerView = registry.view<Transform, Player, Rigidbody, Force>();
            
            for (const auto entity : playerView)
            {
                auto [transform, player, rigidbody, force] = playerView.get<Transform, Player, Rigidbody, Force>(entity);
                
                if (force.GetForceType() == Force::Type::Impulse)
                {
                    const auto cameraTransform = transform.GetChildren()[0].GetComponent<Transform>();
                    auto movement = Vector3::Zero;
                    if (Input::IsKeyDown(KeyCode::W))
                    {
                        movement += cameraTransform.GetForwardDirection();
                        movement.y = 0.0f;
                    }
                    else if (Input::IsKeyDown(KeyCode::S))
                    {
                        movement -= cameraTransform.GetForwardDirection();
                        movement.y = 0.0f;
                    }
                    if (Input::IsKeyDown(KeyCode::A))
                    {
                        movement -= cameraTransform.GetRightDirection();
                        movement.y = 0.0f;
                    }
                    else if (Input::IsKeyDown(KeyCode::D))
                    {
                        movement += cameraTransform.GetRightDirection();
                        movement.y = 0.0f;
                    }
                    if (Input::IsKeyDown(KeyCode::Space))
                    {
                        movement += Vector3::Up;
                    }
                    
                    if (Input::IsKeyDown(KeyCode::LeftShift))
                    {
                        player.Speed = SPRINT_SPEED;
                    }
                    else
                    {
                        player.Speed = WALK_SPEED;
                    }
                    
                    movement.Normalize();
                    force.SetLinearForce(
                        Vector3(
                            deltaTime * player.Speed * movement.x,
                            deltaTime * JUMP_FORCE * movement.y,
                            deltaTime * player.Speed * movement.z
                        )
                    );
                    force.CompleteForce(false);
                }
            }
        }
    };
}