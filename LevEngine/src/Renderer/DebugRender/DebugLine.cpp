#include "levpch.h"
#include "DebugLine.h"

#include "Assets/EngineAssets.h"
#include "Renderer/Renderer3D.h"

namespace LevEngine
{
    void DebugLine::Draw()
    {
        Renderer3D::DrawLineList(Matrix::Identity, m_Mesh, ShaderAssets::Debug());
    }
}

