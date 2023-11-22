#include "pch.h"
#include "ComponentDebugRenderer.h"
#include "Physics/Components/Rigidbody.h"

namespace LevEngine::Editor
{
    class PhysicsColliderComponentDebugRenderer final
        : public ComponentDebugRenderer<Rigidbody, PhysicsColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(Rigidbody& component, const Entity entity) override
        {
            auto transform = entity.GetComponent<Transform>();
            transform.ForceRecalculateModel();
            
            Matrix child;
            Matrix parent =
                Matrix::CreateFromQuaternion(transform.GetWorldRotation())
                * Matrix::CreateTranslation(transform.GetWorldPosition());

            Vector3 transformScale = transform.GetWorldScale();
            const float maxScale = std::max(transformScale.x, std::max(transformScale.y, transformScale.z));
            
            switch (component.GetColliderType())
            {
            case ColliderType::Sphere:
                child =
                    Matrix::CreateScale(component.GetSphereRadius() * maxScale)
                    * Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(Math::ToRadians(component.GetColliderOffsetRotation())))
                    * Matrix::CreateTranslation(component.GetColliderOffsetPosition());
                child *= parent;
                DebugRender::DrawWireSphere(child, Color::Green);
                break;
            case ColliderType::Capsule:
                child =
                    Matrix::CreateScale(maxScale)
                    * Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(Math::ToRadians(component.GetColliderOffsetRotation())))
                    * Matrix::CreateTranslation(component.GetColliderOffsetPosition());
                child *= parent;
                DebugRender::DrawWireCapsule(child, component.GetCapsuleHalfHeight(), component.GetCapsuleRadius(), Color::Green);
                break;
            case ColliderType::Box:
                child =
                    Matrix::CreateScale(component.GetBoxHalfExtents() * 2 * transformScale)
                    * Matrix::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(Math::ToRadians(component.GetColliderOffsetRotation())))
                    * Matrix::CreateTranslation(component.GetColliderOffsetPosition());
                child *= parent;
                DebugRender::DrawWireCube(child, Color::Green); 
                break;
            default:
                break;
            }
        } 
    };
}
