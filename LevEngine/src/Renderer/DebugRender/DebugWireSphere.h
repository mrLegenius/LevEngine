#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
class DebugWireSphere final : public DebugShape
{
public:
    explicit DebugWireSphere(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override
    {
        static auto ring1 = Mesh::CreateRing(Vector3::Right, Vector3::Forward);
        static auto ring2 = Mesh::CreateRing(Vector3::Right, Vector3::Up);
        static auto ring3 = Mesh::CreateRing(Vector3::Up, Vector3::Forward);
        
        Renderer3D::DrawLineStrip(m_Model, ring1, ShaderAssets::Debug());
        Renderer3D::DrawLineStrip(m_Model, ring2, ShaderAssets::Debug());
        Renderer3D::DrawLineStrip(m_Model, ring3, ShaderAssets::Debug());
    }

private:
    Matrix m_Model{};
};
}
