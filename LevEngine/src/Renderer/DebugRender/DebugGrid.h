#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
class DebugGrid final : public DebugShape
{
public:
    explicit DebugGrid(const Matrix& model,
                       Vector3 xAxis, Vector3 yAxis,
                       uint32_t xDivisions, uint32_t yDivisions,
                       const Color color)
        : DebugShape(color), m_Mesh(Mesh::CreateGrid(xAxis, yAxis, xDivisions, yDivisions)), m_Model(model) { }
    void Draw() override
    {
        Renderer3D::DrawLineList(m_Model, m_Mesh, ShaderAssets::Debug());
    }

private:
    Ref<Mesh> m_Mesh;
    Matrix m_Model{};
};
}
