#include "levpch.h"
#include "DebugWireSphere.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/3D/Primitives.h"

void LevEngine::DebugWireSphere::Draw()
{
    static auto ring1 = Primitives::CreateRing(Vector3::Right, Vector3::Forward);
    static auto ring2 = Primitives::CreateRing(Vector3::Right, Vector3::Up);
    static auto ring3 = Primitives::CreateRing(Vector3::Up, Vector3::Forward);
        
    Renderer3D::DrawLineStrip(m_Model, ring1, ShaderAssets::Debug());
    Renderer3D::DrawLineStrip(m_Model, ring2, ShaderAssets::Debug());
    Renderer3D::DrawLineStrip(m_Model, ring3, ShaderAssets::Debug());
}
