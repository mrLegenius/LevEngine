#pragma once

namespace Sandbox
{
    class FPSCameraRotationSystem final : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override
        {
            constexpr auto sensitivity = 45.0f;
            const Vector2 mouseInput = Input::GetMouseDelta();
            const auto delta = mouseInput * sensitivity * deltaTime;

            const auto cameraView = registry.view<Transform, CameraComponent>();
            for (const auto cameraEntity : cameraView)
            {
                auto [cameraTransform, camera] = cameraView.get<Transform, CameraComponent>(cameraEntity);

                if (!camera.IsMain) continue;

                auto cameraRotation = cameraTransform.GetWorldRotation().ToEuler() * Math::RadToDeg;

                cameraRotation.x -= delta.y;
                cameraRotation.x = Math::Clamp(cameraRotation.x, -89.9f, 89.9f);
                    
                cameraTransform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(cameraRotation * Math::DegToRad));
            }
        }
    };
}