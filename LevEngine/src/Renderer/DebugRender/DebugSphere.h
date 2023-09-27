#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
class DebugSphere final : public DebugShape
{
public:
    explicit DebugSphere(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override
    {
        static auto mesh = Mesh::CreateSphere(20);
        Renderer3D::DrawMesh(m_Model, mesh, ShaderAssets::Debug());
    }

private:
    Matrix m_Model{};
};
}
