#include "levpch.h"
#include "DebugGrid.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Renderer3D.h"

void LevEngine::DebugGrid::Draw()
{
    Renderer3D::DrawLineList(m_Model, m_Mesh, ShaderAssets::Debug());
}
