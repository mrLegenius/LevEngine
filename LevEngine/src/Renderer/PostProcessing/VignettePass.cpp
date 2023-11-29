#include "levpch.h"
#include "VignettePass.h"

#include "Renderer/PipelineState.h"
#include "Renderer/RenderCommand.h"

#include "Assets/EngineAssets.h"
#include "Renderer/BlendState.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/RasterizerState.h"

#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    VignettePass::VignettePass(const Ref<RenderTarget>& mainRenderTarget) : m_RenderTarget(mainRenderTarget)
    {
        m_Pipeline = CreateRef<PipelineState>();
        m_Pipeline->SetRenderTarget(mainRenderTarget);
        m_Pipeline->SetShader(ShaderType::Vertex, ShaderAssets::Vignette());
        m_Pipeline->SetShader(ShaderType::Pixel, ShaderAssets::Vignette());
        m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
        m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false });
        m_Pipeline->GetBlendState()->SetBlendMode(BlendMode::AlphaBlending);
    }

    String VignettePass::PassName() { return "Vignette"; }

    bool VignettePass::Begin(entt::registry& registry, RenderParams& params)
    {
        if (!RenderSettings::IsVignetteEnabled) return false;

        m_Pipeline->Bind();
        return RenderPass::Begin(registry, params);
    }

    void VignettePass::Process(entt::registry& registry, RenderParams& params)
    {
        RenderCommand::DrawFullScreenQuad();
    }

    void VignettePass::End(entt::registry& registry, RenderParams& params)
    {
        m_Pipeline->Unbind();
    }

    void VignettePass::SetViewport(const Viewport viewport)
    {
        m_Pipeline->GetRasterizerState().SetViewport(viewport);
    }
}
