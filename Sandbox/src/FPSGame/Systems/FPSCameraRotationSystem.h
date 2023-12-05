#pragma once

constexpr auto ROTATION_SPEED = 45.0f;

namespace Sandbox
{
    class FPSCameraRotationSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto cameraView = registry.view<Transform, CameraComponent>();

            const Vector2 mouse = Input::GetMouseDelta();
			
            for (const auto entity : cameraView)
            {
                auto [cameraTransform, camera] = cameraView.get<Transform, CameraComponent>(entity);

                if (!camera.IsMain) continue;
				
                const auto delta = deltaTime * ROTATION_SPEED * mouse;

                auto rotation = cameraTransform.GetWorldRotation().ToEuler() * Math::RadToDeg;

                rotation.x -= delta.y;
                rotation.y -= delta.x;
                rotation.x = Math::Clamp(rotation.x, -89.99f, 89.999f);
                
                cameraTransform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));
            }
        }
    };
}