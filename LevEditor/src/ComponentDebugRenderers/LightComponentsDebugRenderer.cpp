#include "pch.h"

#include "ComponentDebugRenderer.h"

namespace LevEngine::Editor
{
    class DirectionalLightComponentDebugRenderer final
        : public ComponentDebugRenderer<DirectionalLightComponent, DirectionalLightComponentDebugRenderer>
    {
    protected:
        bool IsRenderedOnlyWhenSelected() override { return true; }
        
        void DrawContent(DirectionalLightComponent& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            constexpr int rays = 8;
            constexpr float radius = 0.2f;
            constexpr float length = 1.0f;
            
            for (int i = 0; i < rays; i++)
            {
                const float angle = static_cast<float>(i) * 360.0f / rays;
                const float x = cos(angle * Math::DegToRad);
                const float y = sin(angle * Math::DegToRad);
                Vector3 offset{x, y, 0};

                Vector3 startPosition = transform.GetWorldPosition() + Vector3::Transform(offset * radius, transform.GetWorldRotation());
                DebugRender::DrawLine(startPosition, startPosition + transform.GetForwardDirection() * length, Color::White);
            }

            DebugRender::DrawCircle(transform.GetWorldPosition(), radius, transform.GetWorldRotation(), Color::White);
        }
    };

    class PointLightComponentDebugRenderer final
        : public ComponentDebugRenderer<PointLightComponent, PointLightComponentDebugRenderer>
    {
    protected:
        bool IsRenderedOnlyWhenSelected() override { return true; }
        
        void DrawContent(PointLightComponent& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            DebugRender::DrawWireSphere(transform.GetWorldPosition(), component.Range, Color::White);
        }
    };
}
