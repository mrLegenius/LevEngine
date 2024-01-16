#include "pch.h"

#include "ComponentDebugRenderer.h"
#include "Scene/Components/NavMesh/NavMeshComponent.h"

namespace LevEngine::Editor
{
    class NavMeshComponentDebugRenderer final
        : public ComponentDebugRenderer<NavMeshComponent, NavMeshComponentDebugRenderer>
    {
    protected:
        bool IsRenderedOnlyWhenSelected() override { return false; }
        
        void DrawContent(NavMeshComponent& component, const Entity entity) override
        {
            if(!component.IsBuilded)
            {
                return;
            }
            
            const auto& mesh = component.GetPolyMesh();
            const int maxVertexPerPolygon = mesh.nvp;
            const float cellSize = mesh.cs;
            const float cellHeight = mesh.ch;
            const float* origin = mesh.bmin;
    		
            // Draw neighbours edges
            const Color neighbourEdgesColor = Color(0, 48 / 255.0f, 64 / 255.0f, 32 / 255.0f);
            for (int i = 0; i < mesh.npolys; ++i)
            {
                const unsigned short* p = &mesh.polys[i * maxVertexPerPolygon * 2];
                for (int j = 0; j < maxVertexPerPolygon; ++j)
                {
                    if (bool isIndexInvalid = p[j] == RC_MESH_NULL_IDX)
                    {
                        break;
                    }
                    if (bool isBorder = p[maxVertexPerPolygon + j] & RC_BORDER_REG)
                    {
                        continue;
                    }

                    const bool isNextIndexInvalid = p[j + 1] == RC_MESH_NULL_IDX;
                    const int nextIndex = (j + 1 >= maxVertexPerPolygon || isNextIndexInvalid) ? 0 : j + 1;
                    const int vi[2] = { p[j], p[nextIndex] };

                    Vector3 points[2];
                    for (int k = 0; k < 2; ++k)
                    {
                        const unsigned short* vertex = &mesh.verts[vi[k] * 3];
                        const float x = origin[0] + vertex[0] * cellSize;
                        const float y = origin[1] + (vertex[1] + 1) * cellHeight + 0.1f;
                        const float z = origin[2] + vertex[2] * cellSize;
                        points[k] = Vector3(x, y, z);
                    }
                    DebugRender::DrawLine(points[0], points[1], neighbourEdgesColor);
                }
            }

            // Draw boundary edges
            const Color boundaryColor = Color(0.9f, 0.1f, 0.1f, 0.8f);
            for (int i = 0; i < mesh.npolys; ++i)
            {
                const unsigned short* p = &mesh.polys[i * maxVertexPerPolygon * 2];
                for (int j = 0; j < maxVertexPerPolygon; ++j)
                {
                    if (const bool isIndexInvalid = p[j] == RC_MESH_NULL_IDX)
                    {
                        break;
                    }
                    if (const bool isNotBorder = (p[maxVertexPerPolygon + j] & RC_BORDER_REG) == 0)
                    {
                        continue;
                    }

                    const bool isNextIndexInvalid = p[j + 1] == RC_MESH_NULL_IDX;
                    const int nextIndex = (j + 1 >= maxVertexPerPolygon || isNextIndexInvalid) ? 0 : j + 1;
                    const int vi[2] = { p[j], p[nextIndex] };

                    Color color = boundaryColor;
                    if (bool isNotWalkable = (p[maxVertexPerPolygon + j] & 0xf) != 0xf)
                    {
                        color = Color(1.0f, 1.0f, 1.0f, 0.5f);
                    }

                    Vector3 points[2];
                    for (int k = 0; k < 2; ++k)
                    {
                        const unsigned short* vertex = &mesh.verts[vi[k] * 3];
                        const float x = origin[0] + vertex[0] * cellSize;
                        const float y = origin[1] + (vertex[1] + 1) * cellHeight + 0.1f;
                        const float z = origin[2] + vertex[2] * cellSize;
                        points[k] = Vector3(x, y, z);
                    }
                    DebugRender::DrawLine(points[0], points[1], color);
                }
            }
        }
    };
}
