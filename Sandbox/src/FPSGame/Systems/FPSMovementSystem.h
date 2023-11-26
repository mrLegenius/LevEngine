#pragma once

namespace Sandbox
{
    class FPSMovementSystem final : public System
    {
        void Update(const float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, Player, Rigidbody>();

            constexpr auto rotationSpeed = 45;
            constexpr auto moveSpeed = 300;
            constexpr auto jumpImpulse = 1000;
            const Vector2 mouse = Input::GetMouseDelta();
			
            for (const auto entity : view)
            {
                auto [transform, player, rigidbody] = view.get<Transform, Player, Rigidbody>(entity);

                const auto delta = mouse * rotationSpeed * deltaTime;

                auto rotation = transform.GetWorldRotation().ToEuler() * Math::RadToDeg;
                rotation.y -= delta.x;
                transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));
                
                if (Input::IsKeyDown(KeyCode::W))
                    rigidbody.AddForce(deltaTime * moveSpeed * transform.GetForwardDirection());
                else if (Input::IsKeyDown(KeyCode::S))
                    rigidbody.AddForce(deltaTime * moveSpeed * -transform.GetForwardDirection());

                if (Input::IsKeyDown(KeyCode::A))
                    rigidbody.AddForce(deltaTime * moveSpeed * -transform.GetRightDirection());
                else if (Input::IsKeyDown(KeyCode::D))
                    rigidbody.AddForce(moveSpeed * deltaTime * transform.GetRightDirection());

                if (Input::IsKeyPressed(KeyCode::Space))
                    rigidbody.AddImpulse(Vector3::Up * jumpImpulse);
            }
        }
    };
}