#include "levpch.h"
#include "DebugWireSphere.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

void LevEngine::DebugWireSphere::Draw()
{
    static auto circle = Primitives::CreateWireSphere();
        
    Renderer3D::DrawLineList(m_Model, circle, ShaderAssets::Debug());
}
