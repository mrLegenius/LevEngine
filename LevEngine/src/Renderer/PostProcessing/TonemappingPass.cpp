#include "levpch.h"
#include "TonemappingPass.h"

#include "Renderer/PipelineState.h"
#include "Renderer/RenderCommand.h"

#include "Assets.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/RasterizerState.h"

#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    TonemappingPass::TonemappingPass(const Ref<RenderTarget>& mainRenderTarget, const Ref<Texture>& colorTexture) : m_ColorTexture(colorTexture)
    {
        m_Pipeline = CreateRef<PipelineState>();
        m_Pipeline->SetRenderTarget(mainRenderTarget);
        m_Pipeline->SetShader(ShaderType::Vertex, ShaderAssets::Tonemapping());
        m_Pipeline->SetShader(ShaderType::Pixel, ShaderAssets::Tonemapping());
        m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
        m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false });
    }

    void TonemappingPass::SetLuminanceMap(const Ref<Texture>& map) { m_LuminanceMap = map; }

    bool TonemappingPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_Pipeline->Bind();
        m_ColorTexture->Bind(0, ShaderType::Pixel);
        m_LuminanceMap->Bind(1, ShaderType::Pixel);
        return RenderPass::Begin(registry, params);
    }

    void TonemappingPass::Process(entt::registry& registry, RenderParams& params)
    {
        RenderCommand::DrawFullScreenQuad();
    }

    void TonemappingPass::End(entt::registry& registry, RenderParams& params)
    {
        m_LuminanceMap->Unbind(1, ShaderType::Pixel);
        m_ColorTexture->Unbind(0, ShaderType::Pixel);
        m_Pipeline->Unbind();
    }

    void TonemappingPass::SetViewport(const Viewport viewport)
    {
        m_Pipeline->GetRasterizerState().SetViewport(viewport);
    }
}
