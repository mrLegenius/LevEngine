#include "levpch.h"
#include "DebugWireCube.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

void LevEngine::DebugWireCube::Draw()
{
    static auto mesh = Primitives::CreateWireCube();
    Renderer3D::DrawLineList(m_Model, mesh, ShaderAssets::Debug());
}
