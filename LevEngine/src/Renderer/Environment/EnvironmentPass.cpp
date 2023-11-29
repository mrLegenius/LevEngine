#include "levpch.h"
#include "EnvironmentPass.h"

namespace LevEngine
{
    EnvironmentPass::EnvironmentPass(const Ref<RenderTarget>& renderTarget)
    {
        m_PrecomputePass = CreateScope<EnvironmentPrecomputePass>();
        m_RenderPass = CreateScope<EnvironmentRenderPass>(renderTarget);
    }

    String EnvironmentPass::PassName() { return "Environment"; }

    void EnvironmentPass::Process(entt::registry& registry, RenderParams& params)
    {
        m_PrecomputePass->Execute(registry, params);

        if (const auto environmentMap = m_PrecomputePass->GetEnvironmentCubemap())
            m_RenderPass->SetEnvironmentMap(environmentMap);

        m_RenderPass->Execute(registry, params);
    }

    void EnvironmentPass::SetViewport(const Viewport viewport)
    {
        m_RenderPass->SetViewport(viewport);
    }
}
