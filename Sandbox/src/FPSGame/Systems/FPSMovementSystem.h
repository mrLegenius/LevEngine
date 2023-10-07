#pragma once

namespace Sandbox
{
    class FPSMovementSystem final : public System
    {
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, Player, Rigidbody>();

            constexpr auto rotationSpeed = 45;
            const Vector2 mouse{ Input::GetMouseDelta().first, Input::GetMouseDelta().second };
			
            for (const auto entity : view)
            {
                auto [transform, player, rigidbody] = view.get<Transform, Player, Rigidbody>(entity);

                const auto delta = mouse * rotationSpeed * deltaTime;

                auto rotation = transform.GetWorldRotation().ToEuler() * Math::RadToDeg;
                rotation.y -= delta.x;
                transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));
				
                if (Input::IsKeyDown(KeyCode::W))
                    transform.MoveForward(deltaTime * 10);
                else if (Input::IsKeyDown(KeyCode::S))
                    transform.MoveBackward(deltaTime * 10);

                if (Input::IsKeyDown(KeyCode::A))
                    transform.MoveLeft(deltaTime * 10);
                else if (Input::IsKeyDown(KeyCode::D))
                    transform.MoveRight(deltaTime * 10);

                if (Input::IsKeyPressed(KeyCode::Space))
                    rigidbody.AddImpulse(Vector3::Up * 10);
            }
        }
    };
}