#include "levpch.h"
#include "BloomPass.h"

#include "Renderer/PipelineState.h"
#include "Renderer/RenderCommand.h"

#include "Assets.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/RasterizerState.h"

namespace LevEngine
{
    void SetViewportForTexture(const Ref<PipelineState>& pipelineState, const Ref<Texture>& texture)
    {
        const Viewport viewport{0, 0, static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight())};
        pipelineState->GetRasterizerState().SetViewport(viewport);
        pipelineState->GetRasterizerState().Bind();
    }
    
    void PostProcess(
        const Ref<PipelineState>& pipelineState,
        const Ref<RenderTarget>& renderTarget,
        const Ref<Shader>& shader,
        const Ref<Texture>& outputTexture,
        const Ref<Texture>& inputTexture0,
        const Ref<Texture>& inputTexture1 = nullptr)
    {
        renderTarget->AttachTexture(AttachmentPoint::Color0, outputTexture);
        renderTarget->Bind();
        SetViewportForTexture(pipelineState, outputTexture);
        if (inputTexture0)
            inputTexture0->Bind(0, ShaderType::Pixel);
        if (inputTexture1)
            inputTexture1->Bind(1, ShaderType::Pixel);
        shader->Bind();
        RenderCommand::DrawFullScreenQuad();
    }
    
    BloomPass::BloomPass(const Ref<Texture>& colorTexture) : m_ColorTexture(colorTexture)
    {
        const auto width = colorTexture->GetWidth();
        const auto height = colorTexture->GetHeight();
        
        {
            const Texture::TextureFormat format
            {
                Texture::Components::RGB,
                Texture::Type::Float,
                1, 11, 11, 10, 0, 0, 0
            };

            m_BloomMap = Texture::CreateTexture2D(width, height, 1, format);
        }

        {
            const Texture::TextureFormat format
            {
                Texture::Components::RGBA,
                Texture::Type::Float,
                1, 16, 16, 16, 16, 0, 0
            };

            m_DownScaleMaps[0] = Texture::CreateTexture2D(width / 2, height / 2, 1, format);
            m_DownScaleMaps[1] = Texture::CreateTexture2D(width / 4, height / 4, 1, format);
            m_DownScaleMaps[2] = Texture::CreateTexture2D(width / 8, height / 8, 1, format);
            m_BlurMap = Texture::CreateTexture2D(width / 8, height / 8, 1, format);
        }

        m_RenderTarget = RenderTarget::Create();
        
        m_Pipeline = CreateRef<PipelineState>();
        m_Pipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_Pipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_Pipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
        m_Pipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false });
    }

    void BloomPass::SetLuminanceMap(const Ref<Texture>& map) { m_LuminanceMap = map; }
    const Ref<Texture>& BloomPass::GetBloomMap() const { return m_DownScaleMaps[0]; }

    bool BloomPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_Pipeline->Bind();
        m_ColorTexture->Bind(0, ShaderType::Pixel);
        m_LuminanceMap->Bind(1, ShaderType::Pixel);
        return RenderPass::Begin(registry, params);
    }

    void BloomPass::Process(entt::registry& registry, RenderParams& params)
    {
        //Render bloom
        PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::Bloom(), m_BloomMap, m_ColorTexture, m_LuminanceMap);

        //Downscale
        PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::Scale(), m_DownScaleMaps[0], m_BloomMap);
        PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::Scale(), m_DownScaleMaps[1], m_DownScaleMaps[0]);
        PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::Scale(), m_DownScaleMaps[2], m_DownScaleMaps[1]);

        //Blur
        for (uint32_t i = 0; i < 4; ++i)
        {
            PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::BloomBlurH(), m_BlurMap, m_DownScaleMaps[2]);
            PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::BloomBlurV(), m_DownScaleMaps[2], m_BlurMap);
        }

        //Upscale
        PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::Scale(), m_DownScaleMaps[1], m_DownScaleMaps[2]);
        PostProcess(m_Pipeline, m_RenderTarget, ShaderAssets::Scale(), m_DownScaleMaps[0], m_DownScaleMaps[1]);
    }

    void BloomPass::End(entt::registry& registry, RenderParams& params)
    {
        m_LuminanceMap->Unbind(1, ShaderType::Pixel);
        m_ColorTexture->Unbind(0, ShaderType::Pixel);
        m_Pipeline->Unbind();
    }

    void BloomPass::SetViewport(const Viewport viewport)
    {
        m_Pipeline->GetRasterizerState().SetViewport(viewport);
    }
}
