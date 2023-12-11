#include "pch.h"
#include "ComponentDebugRenderer.h"

namespace LevEngine::Editor
{
    class SphereColliderComponentDebugRenderer final
        : public ComponentDebugRenderer<LegacySphereCollider, SphereColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(LegacySphereCollider& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            DebugRender::DrawWireSphere(transform.GetWorldPosition() + component.offset, component.radius, Color::Green);
        }
    };

    class BoxColliderComponentDebugRenderer final
       : public ComponentDebugRenderer<LegacyBoxCollider, BoxColliderComponentDebugRenderer>
    {
    protected:
        void DrawContent(LegacyBoxCollider& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            DebugRender::DrawWireCube(transform.GetWorldPosition() + component.offset, component.extents * 2, Color::Green);
        }
    };
}

