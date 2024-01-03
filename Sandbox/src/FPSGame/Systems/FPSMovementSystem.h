#pragma once
#include <complex>

#include "FPSGame/Components/Player.h"
#include "Physics/Physics.h"
#include "Physics/Components/CharacterController.h"

namespace Sandbox
{
    constexpr float MAX_WALK_SPEED = 1.0f;
    constexpr float MAX_SPRINT_SPEED = 2.0f;
    
    class FPSMovementSystem final : public System
    {
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto playerView = registry.view<Transform, Player, CharacterController>();
            
            for (const auto entity : playerView)
            {
                auto [transform, player, controller] = playerView.get<Transform, Player, CharacterController>(entity);
                
                const auto& cameraTransform = transform.GetChildren()[0].GetComponent<Transform>();
                
                if (Input::IsKeyDown(KeyCode::LeftShift))
                {
                    player.Speed = MAX_SPRINT_SPEED;
                }
                else
                {
                    player.Speed = MAX_WALK_SPEED;
                }

                auto displacement = Vector3::Zero;
                const auto gravity = App::Get().GetPhysics().GetGravity();
                
                if (Input::IsKeyDown(KeyCode::W))
                {
                    displacement += cameraTransform.GetForwardDirection();
                }
                else if (Input::IsKeyDown(KeyCode::S))
                {
                    displacement -= cameraTransform.GetForwardDirection();
                }
                if (Input::IsKeyDown(KeyCode::A))
                {
                    displacement -= cameraTransform.GetRightDirection();
                }
                else if (Input::IsKeyDown(KeyCode::D))
                {
                    displacement += cameraTransform.GetRightDirection();
                }
                
                displacement.Normalize();

                displacement *= player.Speed * 0.25f;

                // apply gravity
                if (displacement.y <= 0.0f)
                {
                    displacement.y = gravity.y * deltaTime;
                }
                else
                {
                    displacement.y += gravity.y * 10.0f * deltaTime;
                }

                if (displacement != Vector3::Zero)
                {
                    controller.Move(displacement, deltaTime);
                }
            }
        }
    };
}