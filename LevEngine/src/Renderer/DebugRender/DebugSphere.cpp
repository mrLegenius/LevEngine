#include "levpch.h"
#include "DebugSphere.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

void LevEngine::DebugSphere::Draw()
{
    static auto mesh = Primitives::CreateSphere(20);
    Renderer3D::DrawMesh(m_Model, mesh, ShaderAssets::Debug());
}
