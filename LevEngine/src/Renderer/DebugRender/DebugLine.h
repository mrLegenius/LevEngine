#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

namespace LevEngine
{
class DebugLine final : public DebugShape
{
public:
    explicit DebugLine(const Vector3 start, const Vector3 end, const Color color)
        : DebugShape(color), m_Mesh(Primitives::CreateLine(start, end)) { }
    
    void Draw() override
    {
        Renderer3D::DrawLineList(Matrix::Identity, m_Mesh, ShaderAssets::Debug());
    }

private:
    Ref<Mesh> m_Mesh;
};
}
