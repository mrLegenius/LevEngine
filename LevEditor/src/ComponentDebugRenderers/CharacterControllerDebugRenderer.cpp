#include "pch.h"
#include "ComponentDebugRenderer.h"
#include "Physics/Components/CharacterController.h"

namespace LevEngine::Editor
{
    class CharacterControllerColliderComponentDebugRenderer final
        : public ComponentDebugRenderer<CharacterController, CharacterControllerColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(CharacterController& component, const Entity entity) override
        {
            const auto transform = entity.GetComponent<Transform>();

            // turn the debug capsule into an upright position
            const auto controllerRotationCorrectionMatrix =
                Matrix::CreateFromAxisAngle(Vector3::Forward, Math::ToRadians(90.0f));

            const auto controllerCenterOffsetMatrix =
                Matrix::CreateTranslation(component.GetCenterOffset());

            const auto transformScale = transform.GetWorldScale();
            const auto maxTransformScale = Math::MaxElement(transformScale);
            const auto transformScaleMatrix = Matrix::CreateScale(maxTransformScale);

            const auto transformRotationTranslationMatrix =
                Matrix::CreateFromQuaternion(transform.GetWorldRotation())
                * Matrix::CreateTranslation(transform.GetWorldPosition());
            
            auto model =
                    controllerRotationCorrectionMatrix
                    * controllerCenterOffsetMatrix
                    * transformScaleMatrix
                    * transformRotationTranslationMatrix;
            
            DebugRender::DrawWireCapsule(
                model,
                component.GetHeight() / 2.0f,
                component.GetRadius(),
                Color::Pink
            );
        } 
    };
}