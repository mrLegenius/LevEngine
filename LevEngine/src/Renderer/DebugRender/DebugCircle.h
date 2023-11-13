#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

namespace LevEngine
{
class DebugCircle final : public DebugShape
{
public:
    explicit DebugCircle(const Matrix& model, const Color color)
    : DebugShape(color), m_Model(model) { }
    
    void Draw() override
    {
        const auto right = Vector3::Transform(Vector3::Right, m_Rotation);
        const auto up = Vector3::Transform(Vector3::Up, m_Rotation);
        static auto ring = Primitives::CreateRing(Vector3::Right, Vector3::Up);
        Renderer3D::DrawLineStrip(m_Model, ring, ShaderAssets::Debug());
    }

private:
    Matrix m_Model{};
    Quaternion m_Rotation{};
};
}
