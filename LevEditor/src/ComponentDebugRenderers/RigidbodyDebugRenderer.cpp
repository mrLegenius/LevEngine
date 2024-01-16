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
            
            Matrix colliderModel;
            Matrix transformModel =
                Matrix::CreateFromQuaternion(transform.GetWorldRotation())
                * Matrix::CreateTranslation(transform.GetWorldPosition());

            Matrix colliderRotationTranslationModel =
                Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(Math::ToRadians(component.GetColliderOffsetRotation())))
                * Matrix::CreateTranslation(component.GetColliderOffsetPosition());
            
            Vector3 transformScale = transform.GetWorldScale();
            const float maxScale = Math::MaxElement(transformScale);
            
            switch (component.GetColliderType())
            {
            case Collider::Type::Sphere:
                colliderModel =
                    Matrix::CreateScale(component.GetSphereRadius() * maxScale)
                    * colliderRotationTranslationModel;
                colliderModel *= transformModel;
                DebugRender::DrawWireSphere(
                    colliderModel,
                    Color::Green
                );
                break;
                
            case Collider::Type::Capsule:
                colliderModel =
                    Matrix::CreateScale(maxScale)
                    * colliderRotationTranslationModel;
                colliderModel *= transformModel;
                DebugRender::DrawWireCapsule(
                    colliderModel,
                    component.GetCapsuleHalfHeight(),
                    component.GetCapsuleRadius(),
                    Color::Green
                );
                break;
                
            case Collider::Type::Box:
                colliderModel =
                    Matrix::CreateScale(component.GetBoxHalfExtents() * 2 * transformScale)
                    * colliderRotationTranslationModel;
                colliderModel *= transformModel;
                DebugRender::DrawWireCube(
                    colliderModel,
                    Color::Green
                ); 
                break;
                
            default:
                break;
            }
        } 
    };
}