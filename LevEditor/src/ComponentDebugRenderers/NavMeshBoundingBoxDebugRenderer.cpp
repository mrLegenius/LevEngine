#include "pch.h"

#include "ComponentDebugRenderer.h"
#include "Scene/Components/NavMesh/NavMeshBoundingBox.h"

namespace LevEngine::Editor
{
    class NavMeshBoundingBoxDebugRenderer final
        : public ComponentDebugRenderer<NavMeshBoundingBox, NavMeshBoundingBoxDebugRenderer>
    {
    protected:
        bool IsRenderedOnlyWhenSelected() override { return true; }
        
        void DrawContent(NavMeshBoundingBox& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            const auto& offset = component.GetPositionOffset();
            const auto& boxHalfExtents = component.GetBoxHalfExtents();
            const Vector3& center = Vector3::Transform(offset, transform.GetModel());
            const Vector3& point1 =  {center.x - boxHalfExtents.x,
                center.y - boxHalfExtents.y,
                center.z - boxHalfExtents.z};
            const Vector3& point2 =  {center.x + boxHalfExtents.x,
                center.y - boxHalfExtents.y,
                center.z - boxHalfExtents.z};
            const Vector3& point3 =  {center.x + boxHalfExtents.x,
                center.y + boxHalfExtents.y,
                center.z - boxHalfExtents.z};
            const Vector3& point4 =  {center.x - boxHalfExtents.x,
                center.y + boxHalfExtents.y,
                center.z - boxHalfExtents.z};
            const Vector3& point5 =  {center.x - boxHalfExtents.x,
                center.y - boxHalfExtents.y,
                center.z + boxHalfExtents.z};
            const Vector3& point6 =  {center.x + boxHalfExtents.x,
                center.y - boxHalfExtents.y,
                center.z + boxHalfExtents.z};
            const Vector3& point7 =  {center.x + boxHalfExtents.x,
                center.y + boxHalfExtents.y,
                center.z + boxHalfExtents.z};
            const Vector3& point8 =  {center.x - boxHalfExtents.x,
                center.y + boxHalfExtents.y,
                center.z + boxHalfExtents.z};
            DebugRender::DrawLine(point1, point2, Color::Green);
            DebugRender::DrawLine(point2, point3, Color::Green);
            DebugRender::DrawLine(point3, point4, Color::Green);
            DebugRender::DrawLine(point4, point1, Color::Green);
            DebugRender::DrawLine(point5, point6, Color::Green);
            DebugRender::DrawLine(point6, point7, Color::Green);
            DebugRender::DrawLine(point7, point8, Color::Green);
            DebugRender::DrawLine(point8, point5, Color::Green);
            DebugRender::DrawLine(point1, point5, Color::Green);
            DebugRender::DrawLine(point2, point6, Color::Green);
            DebugRender::DrawLine(point3, point7, Color::Green);
            DebugRender::DrawLine(point4, point8, Color::Green);
        }
    };
}
