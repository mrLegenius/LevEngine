#include "pch.h"
#include "ComponentDebugRenderer.h"
#include "Physics/PhysX/PhysicsRigidbody.h"

namespace LevEngine::Editor
{
    class SphereColliderComponentDebugRenderer final
        : public ComponentDebugRenderer<SphereCollider, SphereColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(SphereCollider& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            DebugRender::DrawWireSphere(transform.GetWorldPosition() + component.offset, component.radius, Color::Green);
        }
    };

    class BoxColliderComponentDebugRenderer final
       : public ComponentDebugRenderer<BoxCollider, BoxColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(BoxCollider& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            DebugRender::DrawWireCube(transform.GetWorldPosition() + component.offset, transform.GetWorldRotation(), component.extents * 2, Color::Green);
        }
    }; 

    class PhysicsColliderComponentDebugRenderer final
    : public ComponentDebugRenderer<PhysicsRigidbody, PhysicsColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(PhysicsRigidbody& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            switch (component.GetColliderType())
            {
            case ColliderType::Sphere:
                DebugRender::DrawWireSphere(transform.GetWorldPosition() + component.GetColliderOffsetPosition(), component.GetCapsuleRadius(), Color::Green);
                break;
            case ColliderType::Capsule:
                //TODO: CAPSULE DRAWER IMPLEMENTATION
                break;
            case ColliderType::Box:
                DebugRender::DrawWireCube(
                    transform.GetWorldPosition(),
                    Quaternion::CreateFromYawPitchRoll(Math::ToRadians(component.GetColliderOffsetRotation())) * transform.GetLocalRotation(),
                    component.GetBoxHalfExtents() * 2,
                    Color::Green
                );
                break;
            default:
                break;
            }
        }
    };
}

