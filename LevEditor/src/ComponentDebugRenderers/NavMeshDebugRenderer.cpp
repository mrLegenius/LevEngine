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
            const int nvp = mesh.nvp;
            const float cs = mesh.cs;
            const float ch = mesh.ch;
            const float* orig = mesh.bmin;
    		
            // Draw neighbours edges
            Color coln = Color(0, 48 / 255.0f, 64 / 255.0f, 32 / 255.0f);
            for (int i = 0; i < mesh.npolys; ++i)
            {
                const unsigned short* p = &mesh.polys[i * nvp * 2];
                for (int j = 0; j < nvp; ++j)
                {
                    if (p[j] == RC_MESH_NULL_IDX) break;
                    if (p[nvp + j] & 0x8000) continue;
                    const int nj = (j + 1 >= nvp || p[j + 1] == RC_MESH_NULL_IDX) ? 0 : j + 1;
                    const int vi[2] = { p[j], p[nj] };

                    DirectX::SimpleMath::Vector3 points[2];
                    for (int k = 0; k < 2; ++k) {
                        const unsigned short* v = &mesh.verts[vi[k] * 3];
                        const float x = orig[0] + v[0] * cs;
                        const float y = orig[1] + (v[1] + 1) * ch + 0.1f;
                        const float z = orig[2] + v[2] * cs;
                        points[k] = DirectX::SimpleMath::Vector3(x, y, z);
                    }
                    DebugRender::DrawLine(points[0], points[1], coln);
                }
            }

            // Draw boundary edges
            Color colb = Color(0.9f, 0.1f, 0.1f, 0.8f);
            for (int i = 0; i < mesh.npolys; ++i)
            {
                const unsigned short* p = &mesh.polys[i * nvp * 2];
                for (int j = 0; j < nvp; ++j)
                {
                    if (p[j] == RC_MESH_NULL_IDX) break;
                    if ((p[nvp + j] & 0x8000) == 0) continue;
                    const int nj = (j + 1 >= nvp || p[j + 1] == RC_MESH_NULL_IDX) ? 0 : j + 1;
                    const int vi[2] = { p[j], p[nj] };

                    Color col = colb;
                    if ((p[nvp + j] & 0xf) != 0xf)
                        col = Color(1.0f, 1.0f, 1.0f, 0.5f);

                    DirectX::SimpleMath::Vector3 points[2];
                    for (int k = 0; k < 2; ++k)
                    {
                        const unsigned short* v = &mesh.verts[vi[k] * 3];
                        const float x = orig[0] + v[0] * cs;
                        const float y = orig[1] + (v[1] + 1) * ch + 0.1f;
                        const float z = orig[2] + v[2] * cs;
                        points[k] = DirectX::SimpleMath::Vector3(x, y, z);
                    }
                    DebugRender::DrawLine(points[0], points[1], col);
                }
            }
        }
    };
}
