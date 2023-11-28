#include "levpch.h"
#include "LuminanceAdaptationPass.h"

#include "Assets.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/PipelineState.h"
#include "Renderer/RasterizerState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderTarget.h"

namespace LevEngine
{
    LuminanceAdaptationPass::LuminanceAdaptationPass()
    {
        const Texture::TextureFormat format(
            Texture::Components::R,
            Texture::Type::Float,
            1,
            32, 0, 0, 0, 0, 0);
        
        m_LuminanceMapBuffer[0] = Texture::CreateTexture2D(1, 1, 1, format);
        m_LuminanceMapBuffer[1] = Texture::CreateTexture2D(1, 1, 1, format);

        m_RenderTarget = RenderTarget::Create();

        m_Pipeline = CreateRef<PipelineState>();
        m_Pipeline->SetRenderTarget(m_RenderTarget);
        m_Pipeline->SetShader(ShaderType::Vertex, ShaderAssets::LuminanceAdaptation());
        m_Pipeline->SetShader(ShaderType::Pixel, ShaderAssets::LuminanceAdaptation());
        m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
        m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false });
        m_Pipeline->GetRasterizerState().SetViewport({0, 0, 1, 1});
    }

    String LuminanceAdaptationPass::PassName() { return "Luminance Adaptation"; }

    bool LuminanceAdaptationPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_Pipeline->GetRenderTarget()->AttachTexture(AttachmentPoint::Color0, m_LuminanceMapBuffer[m_CurrentMapInBuffer]);
        m_Pipeline->Bind(); 
        m_LuminanceMapBuffer[!m_CurrentMapInBuffer]->Bind(0, ShaderType::Pixel);
        m_LuminanceMap->Bind(1, ShaderType::Pixel);
        return RenderPass::Begin(registry, params);
    }

    void LuminanceAdaptationPass::Process(entt::registry& registry, RenderParams& params)
    {
        RenderCommand::DrawFullScreenQuad();
    }

    void LuminanceAdaptationPass::End(entt::registry& registry, RenderParams& params)
    {
        m_LuminanceMapBuffer[!m_CurrentMapInBuffer]->Unbind(0, ShaderType::Pixel);
        m_LuminanceMap->Unbind(1, ShaderType::Pixel);
        m_Pipeline->Unbind();
    }
}
