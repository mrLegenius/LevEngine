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

            Matrix colliderModel;
            const Matrix transformModel =
                Matrix::CreateFromQuaternion(transform.GetWorldRotation())
                * Matrix::CreateTranslation(transform.GetWorldPosition());

            Matrix controllerTranslationModel =
                Matrix::CreateTranslation(component.GetCenterOffset());
            
            Vector3 transformScale = transform.GetWorldScale();
            const float maxTransformScale = Math::MaxElement(transformScale);

            colliderModel =
                Matrix::CreateScale(maxTransformScale);
            // turn the debug capsule into an upright position
            colliderModel *= Matrix::CreateFromAxisAngle(Vector3::Forward, Math::ToRadians(90.0f));
            colliderModel *= controllerTranslationModel;
            colliderModel *= transformModel;
            DebugRender::DrawWireCapsule(
                colliderModel,
                component.GetHeight() / 2.0f,
                component.GetRadius(),
                Color::Pink
            );
        } 
    };
}