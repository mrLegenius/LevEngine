#include "levpch.h"
#include "LuminancePass.h"

#include "Renderer/PipelineState.h"
#include "Renderer/RenderCommand.h"

#include "Assets.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/RasterizerState.h"

#include "Renderer/RenderSettings.h"
#include "Renderer/RenderTarget.h"

namespace LevEngine
{
    LuminancePass::LuminancePass(const Ref<Texture>& colorTexture) : m_ColorTexture(colorTexture)
    {
        const Texture::TextureFormat format(
            Texture::Components::R,
            Texture::Type::Float,
            1,
            32, 0, 0, 0, 0, 0);

        const auto luminanceMap = Texture::CreateTexture2D(
            RenderSettings::LuminanceMapSize, RenderSettings::LuminanceMapSize, 1,
            format, CPUAccess::None, false, true);
            
        m_RenderTarget = RenderTarget::Create();
        m_RenderTarget->AttachTexture(AttachmentPoint::Color0, luminanceMap);

        m_Pipeline = CreateRef<PipelineState>();
        m_Pipeline->SetRenderTarget(m_RenderTarget);
        m_Pipeline->SetShader(ShaderType::Vertex, ShaderAssets::Luminance());
        m_Pipeline->SetShader(ShaderType::Pixel, ShaderAssets::Luminance());
        m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
        m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false });
        m_Pipeline->GetRasterizerState().SetViewport({0, 0, RenderSettings::LuminanceMapSize, RenderSettings::LuminanceMapSize});
    }

    Ref<Texture> LuminancePass::GetLuminanceMap() const { return m_RenderTarget->GetTexture(AttachmentPoint::Color0); }

    bool LuminancePass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_Pipeline->Bind();
        m_ColorTexture->Bind(0, ShaderType::Pixel);
        return RenderPass::Begin(registry, params);
    }

    void LuminancePass::Process(entt::registry& registry, RenderParams& params)
    {
        RenderCommand::DrawFullScreenQuad();

        m_Pipeline->GetRenderTarget()->GetTexture(AttachmentPoint::Color0)->GenerateMipMaps();
    }

    void LuminancePass::End(entt::registry& registry, RenderParams& params)
    {
        m_ColorTexture->Unbind(0, ShaderType::Pixel);
        m_Pipeline->Unbind();
    }
}
