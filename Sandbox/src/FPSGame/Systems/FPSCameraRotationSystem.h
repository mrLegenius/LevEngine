#pragma once

namespace Sandbox
{
    class FPSCameraRotationSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto view = registry.view<Transform, CameraComponent>();

            const Vector2 mouse{ Input::GetMouseDelta().first, Input::GetMouseDelta().second };
			
            for (const auto entity : view)
            {
                constexpr auto rotationSpeed = 45;
                auto [transform, camera] = view.get<Transform, CameraComponent>(entity);

                if (!camera.isMain) continue;
				
                const auto delta = mouse * rotationSpeed * deltaTime;

                auto rotation = transform.GetWorldRotation().ToEuler() * Math::RadToDeg;
                rotation.x -= delta.y;

                rotation.x = Math::Clamp(rotation.x, -89.99f, 89.999f);
                transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));
            }
        }
    };
}