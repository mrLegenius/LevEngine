#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

namespace LevEngine
{
class DebugCube final : public DebugShape
{
public:
    explicit DebugCube(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override
    {
        static auto mesh = Primitives::CreateCube();
        Renderer3D::DrawMesh(m_Model, mesh, ShaderAssets::Debug());
    }

private:
    Matrix m_Model{};
};
}
