#include "pch.h"
#include "ComponentDebugRenderer.h"

namespace LevEngine::Editor
{
    class RigidbodyColliderComponentDebugRenderer final
        : public ComponentDebugRenderer<Rigidbody, RigidbodyColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(Rigidbody& component, const Entity entity) override
        {
            auto transform = entity.GetComponent<Transform>();
            const auto transformScale = transform.GetWorldScale();
            const auto maxTransformScale = Math::MaxElement(transformScale);

            auto colliderRotationQuaternion =
                Quaternion::CreateFromYawPitchRoll(Math::ToRadians(component.GetColliderOffsetRotation()));
            auto colliderRotationTranslationOffsetMatrix =
                Matrix::CreateFromQuaternion(colliderRotationQuaternion)
                * Matrix::CreateTranslation(component.GetColliderOffsetPosition());

            auto transformRotationTranslationMatrix =
                Matrix::CreateFromQuaternion(transform.GetWorldRotation())
                * Matrix::CreateTranslation(transform.GetWorldPosition());

            Matrix model = Matrix::Identity;
            switch (component.GetColliderType())
            {
            case Collider::Type::Sphere:
                model =
                    Matrix::CreateScale(component.GetSphereRadius() * maxTransformScale)
                    * colliderRotationTranslationOffsetMatrix
                    * transformRotationTranslationMatrix;
                DebugRender::DrawWireSphere(
                    model,
                    Color::Green
                );
                break;
            case Collider::Type::Capsule:
                model =
                    Matrix::CreateScale(maxTransformScale)
                    * colliderRotationTranslationOffsetMatrix
                    * transformRotationTranslationMatrix;
                DebugRender::DrawWireCapsule(
                    model,
                    component.GetCapsuleHalfHeight(),
                    component.GetCapsuleRadius(),
                    Color::Green
                );
                break;
            case Collider::Type::Box:
                model =
                    Matrix::CreateScale(component.GetBoxHalfExtents() * 2 * transformScale)
                    * colliderRotationTranslationOffsetMatrix
                    * transformRotationTranslationMatrix;
                DebugRender::DrawWireCube(
                    model,
                    Color::Green
                ); 
                break;
            default:
                break;
            }
        } 
    };
}