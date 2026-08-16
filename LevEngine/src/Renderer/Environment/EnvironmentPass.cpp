#include "levpch.h"
#include "EnvironmentPass.h"

namespace LevEngine
{
    EnvironmentPass::EnvironmentPass(const Ref<RenderTarget>& renderTarget)
    {
        m_Atmosphere = CreateRef<AtmosphereConstants>();
        m_PrecomputePass = CreateScope<EnvironmentPrecomputePass>(m_Atmosphere);
        m_RenderPass = CreateScope<EnvironmentRenderPass>(renderTarget, m_Atmosphere);
    }

    String EnvironmentPass::PassName() { return "Environment"; }

    void EnvironmentPass::Process(entt::registry& registry, RenderParams& params)
    {
        // A scene with an AtmosphereComponent renders a procedural sky and builds its image based
        // lighting from it; otherwise the skybox texture path runs unchanged.
        m_Atmosphere->Collect(registry, params);
        m_Atmosphere->Bind();

        m_PrecomputePass->Execute(registry, params);

        if (const auto environmentMap = m_PrecomputePass->GetEnvironmentCubemap())
            m_RenderPass->SetEnvironmentMap(environmentMap);

        m_RenderPass->SetSkyboxCubemap(m_PrecomputePass->GetSkyboxCubemap());

        m_RenderPass->Execute(registry, params);
    }

    void EnvironmentPass::SetViewport(const Viewport viewport)
    {
        m_RenderPass->SetViewport(viewport);
    }
}
