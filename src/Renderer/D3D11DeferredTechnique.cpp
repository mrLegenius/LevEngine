#include "D3D11DeferredTechnique.h"

#include <cassert>
#include <cstdint>
#include <SimpleMath.h>
#include <wrl/client.h>
#include <Assets.h>

#include "Debugging/Profiler.h"
#include "Kernel/Application.h"

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;
extern IDXGISwapChain* swapChain;

D3D11DeferredTechnique::D3D11DeferredTechnique(const uint32_t width, const uint32_t height)
{
    LEV_PROFILE_FUNCTION();

    //Depth Texture
    {
        Texture::TextureFormat depthStencilTextureFormat(
            Texture::Components::DepthStencil,
            Texture::Type::UnsignedNormalized,
            1,
            0, 0, 0, 0, 24, 8);
        m_DepthTexture = D3D11Texture::CreateTexture2D(width, height, 1, depthStencilTextureFormat);
    }

    //Diffuse
    {
        Texture::TextureFormat diffuseTextureFormat(
            Texture::Components::RGBA,
            Texture::Type::UnsignedNormalized,
            1,
            8, 8, 8, 8, 0, 0);
        m_DiffuseTexture = D3D11Texture::CreateTexture2D(width, height, 1, diffuseTextureFormat);
    }

    //Specular
    {
        Texture::TextureFormat specularTextureFormat(
            Texture::Components::RGBA,
            Texture::Type::UnsignedNormalized,
            1,
            8, 8, 8, 8, 0, 0);
        m_SpecularTexture = D3D11Texture::CreateTexture2D(width, height, 1, specularTextureFormat);
    }

    //Normal
    {
        Texture::TextureFormat normalTextureFormat(
            Texture::Components::RGBA,
            Texture::Type::Float,
            1,
            32, 32, 32, 32, 0, 0);
        m_NormalTexture = D3D11Texture::CreateTexture2D(width, height, 1, normalTextureFormat);
    }

    auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();

    m_DepthOnlyRenderTarget = CreateRef<D3D11RenderTarget>();
    m_DepthOnlyRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));

    auto additiveBlending = BlendMode{ true, false, BlendFactor::One, BlendFactor::One};

    m_GBufferRenderTarget = CreateRef<D3D11RenderTarget>();
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color0, mainRenderTarget->GetTexture(AttachmentPoint::Color0));
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color1, m_DiffuseTexture);
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color2, m_SpecularTexture);
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color3, m_NormalTexture);
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, m_DepthTexture);

    m_GeometryPipeline.SetRenderTarget(m_GBufferRenderTarget);
    m_GeometryPipeline.GetRasterizerState().SetCullMode(CullMode::Back);

    //Pipeline1 for point and spot lights
    {
        m_PositionalLightPipeline1.SetRenderTarget(m_DepthOnlyRenderTarget);
        m_PositionalLightPipeline1.GetRasterizerState().SetCullMode(CullMode::Back);
        m_PositionalLightPipeline1.GetRasterizerState().SetDepthClipEnabled(true);

        DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::Greater };
        m_PositionalLightPipeline1.GetDepthStencilState().SetDepthMode(depthMode);

        StencilMode stencilMode{ true };
        FaceOperation faceOperation;
        faceOperation.StencilFunction = CompareFunction::Always;
        faceOperation.StencilDepthPass = StencilOperation::DecrementClamp;
        stencilMode.StencilReference = 1;
        stencilMode.FrontFace = faceOperation;

        m_PositionalLightPipeline1.GetDepthStencilState().SetStencilMode(stencilMode);
    }

    //Pipeline2 for point and spot lights
    {
        m_PositionalLightPipeline2.SetRenderTarget(mainRenderTarget);
        m_PositionalLightPipeline2.GetRasterizerState().SetCullMode(CullMode::Front);
        m_PositionalLightPipeline2.GetRasterizerState().SetDepthClipEnabled(false);

        m_PositionalLightPipeline2.GetBlendState().SetBlendMode(additiveBlending);

        DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::GreaterOrEqual };
        m_PositionalLightPipeline2.GetDepthStencilState().SetDepthMode(depthMode);

        StencilMode stencilMode{ true };
        FaceOperation faceOperation;
        faceOperation.StencilFunction = CompareFunction::Equal;
        faceOperation.StencilDepthPass = StencilOperation::Keep;
        stencilMode.StencilReference = 1;
        stencilMode.BackFace = faceOperation;

        m_PositionalLightPipeline2.GetDepthStencilState().SetStencilMode(stencilMode);
    }

    //Skybox
    {
        m_SkyboxPipeline.SetRenderTarget(mainRenderTarget);

        DepthMode depthMode{ true, DepthWrite::Enable, CompareFunction::LessOrEqual };
        m_SkyboxPipeline.GetRasterizerState().SetCullMode(CullMode::None);
        m_SkyboxPipeline.GetRasterizerState().SetDepthClipEnabled(false);
        m_SkyboxPipeline.GetDepthStencilState().SetDepthMode(depthMode);
    }
}

D3D11DeferredTechnique::~D3D11DeferredTechnique()
{

}


void D3D11DeferredTechnique::BindTextures()
{
    LEV_PROFILE_FUNCTION();

    m_DiffuseTexture->Bind(1);
    m_SpecularTexture->Bind(2);
    m_NormalTexture->Bind(3);
    m_DepthTexture->Bind(4);
}

void D3D11DeferredTechnique::UnbindTextures()
{
    LEV_PROFILE_FUNCTION();

    context->PSSetShaderResources(0, 0, nullptr);
    context->PSSetShaderResources(1, 0, nullptr);
    context->PSSetShaderResources(2, 0, nullptr);
    context->PSSetShaderResources(3, 0, nullptr);
    context->PSSetShaderResources(4, 0, nullptr);

    context->PSSetSamplers(0, 0, nullptr);
}

void D3D11DeferredTechnique::StartOpaquePass()
{
    LEV_PROFILE_FUNCTION();

	m_GBufferRenderTarget->Clear();
    m_GeometryPipeline.Bind();
}

void D3D11DeferredTechnique::StartPositionalLightingPass1()
{
    LEV_PROFILE_FUNCTION();

    context->ClearDepthStencilView(m_DepthOnlyRenderTarget->GetTexture(AttachmentPoint::DepthStencil)->GetDepthStencilView(), D3D11_CLEAR_STENCIL, 1.0f, 1);
    {
        LEV_PROFILE_SCOPE("Pipeline 1");
        m_PositionalLightPipeline1.Bind();
    }
    
    ShaderAssets::DeferredPointLight()->Unbind();
    ShaderAssets::DeferredVertexOnly()->Bind();
}

void D3D11DeferredTechnique::StartPositionalLightingPass2()
{
    LEV_PROFILE_FUNCTION();

    BindTextures();
    m_PositionalLightPipeline1.Unbind();
    m_PositionalLightPipeline2.Bind();
    ShaderAssets::DeferredPointLight()->Unbind();
    ShaderAssets::DeferredPointLight()->Bind();
}

void D3D11DeferredTechnique::StartDirectionalLightingPass()
{
    LEV_PROFILE_FUNCTION();

    Ref<D3D11Texture> depthStencilBuffer = m_DepthOnlyRenderTarget->GetTexture(AttachmentPoint::DepthStencil);

    if (depthStencilBuffer)
        depthStencilBuffer->Copy(m_DepthTexture);

    BindTextures();
}

void D3D11DeferredTechnique::EndLightningPass()
{
    LEV_PROFILE_FUNCTION();

    m_PositionalLightPipeline2.Unbind();
    UnbindTextures();
}
