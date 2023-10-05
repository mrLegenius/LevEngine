#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

namespace LevEngine
{
class DebugWireCube final : public DebugShape
{
public:
    explicit DebugWireCube(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override
    {
        static auto mesh = Primitives::CreateWireCube();
        Renderer3D::DrawLineList(m_Model, mesh, ShaderAssets::Debug());
    }

private:
    Matrix m_Model{};
};
}
