#include "levpch.h"
#include "DebugCircle.h"

#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

void LevEngine::DebugCircle::Draw()
{
    const auto right = Vector3::Transform(Vector3::Right, m_Rotation);
    const auto up = Vector3::Transform(Vector3::Up, m_Rotation);
    static auto ring = Primitives::CreateRing(Vector3::Right, Vector3::Up);
    Renderer3D::DrawLineStrip(m_Model, ring, ShaderAssets::Debug());
}
