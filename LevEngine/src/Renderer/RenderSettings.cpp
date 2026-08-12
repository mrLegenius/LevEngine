#include "levpch.h"
#include "RenderSettings.h"

namespace LevEngine
{
    RenderTechniqueType RenderSettings::RenderTechnique = RenderTechniqueType::Deferred;

    RendererAPI RenderSettings::RendererAPI = RendererAPI::D3D11;

    bool RenderSettings::UseFrustumCulling = true;

    bool RenderSettings::UseInstancing = true;
}
