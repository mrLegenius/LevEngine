#include "levpch.h"
#include "DebugCube.h"

#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

void LevEngine::DebugCube::Draw()
{
    static auto mesh = Primitives::CreateCube();
    Renderer3D::DrawMesh(m_Model, mesh, ShaderAssets::Debug());
}
