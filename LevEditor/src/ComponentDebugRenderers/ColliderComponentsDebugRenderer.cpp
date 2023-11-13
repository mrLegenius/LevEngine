#include "pch.h"
#include "ComponentDebugRenderer.h"

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
            DebugRender::DrawWireCube(transform.GetWorldPosition() + component.offset, component.extents * 2, Color::Green);
        }
    };
}

