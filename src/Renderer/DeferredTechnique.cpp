#include "pch.h"
#include "DeferredTechnique.h"

#include <SimpleMath.h>
#include <Assets.h>

#include "Kernel/Application.h"

namespace LevEngine
{

DeferredTechnique::DeferredTechnique(const uint32_t width, const uint32_t height)
{
    LEV_PROFILE_FUNCTION();

    //Depth Texture
    {
        Texture::TextureFormat depthStencilTextureFormat(
            Texture::Components::DepthStencil,
            Texture::Type::UnsignedNormalized,
            1,
            0, 0, 0, 0, 24, 8);
        m_DepthTexture = Texture::CreateTexture2D(width, height, 1, depthStencilTextureFormat);
    }

    //Diffuse
    {
        Texture::TextureFormat diffuseTextureFormat(
            Texture::Components::RGBA,
            Texture::Type::UnsignedNormalized,
            1,
            8, 8, 8, 8, 0, 0);
        m_DiffuseTexture = Texture::CreateTexture2D(width, height, 1, diffuseTextureFormat);
    }

    //Specular
    {
        Texture::TextureFormat specularTextureFormat(
            Texture::Components::RGBA,
            Texture::Type::UnsignedNormalized,
            1,
            8, 8, 8, 8, 0, 0);
        m_SpecularTexture = Texture::CreateTexture2D(width, height, 1, specularTextureFormat);
    }

    //Normal
    {
        Texture::TextureFormat normalTextureFormat(
            Texture::Components::RGBA,
            Texture::Type::Float,
            1,
            32, 32, 32, 32, 0, 0);
        m_NormalTexture = Texture::CreateTexture2D(width, height, 1, normalTextureFormat);
    }

    auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();

    m_DepthOnlyRenderTarget = RenderTarget::Create();
    m_DepthOnlyRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));

    m_GBufferRenderTarget = RenderTarget::Create();
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color0, mainRenderTarget->GetTexture(AttachmentPoint::Color0));
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color1, m_DiffuseTexture);
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color2, m_SpecularTexture);
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color3, m_NormalTexture);
    m_GBufferRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, m_DepthTexture);

    m_GeometryPipeline.SetRenderTarget(m_GBufferRenderTarget);
    m_GeometryPipeline.GetRasterizerState().SetCullMode(CullMode::Back);
    m_GeometryPipeline.SetShader(Shader::Type::Vertex, ShaderAssets::GBufferPass());
    m_GeometryPipeline.SetShader(Shader::Type::Pixel, ShaderAssets::GBufferPass());

    //Pipeline1 for point and spot lights
    {
        m_PositionalLightPipeline1.SetRenderTarget(m_DepthOnlyRenderTarget);
        m_PositionalLightPipeline1.GetRasterizerState().SetCullMode(CullMode::Back);
        m_PositionalLightPipeline1.GetRasterizerState().SetDepthClipEnabled(true);

        DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::Greater };
        m_PositionalLightPipeline1.GetDepthStencilState()->SetDepthMode(depthMode);

        StencilMode stencilMode{ true };
        FaceOperation faceOperation;
        faceOperation.StencilFunction = CompareFunction::Always;
        faceOperation.StencilDepthPass = StencilOperation::DecrementClamp;
        stencilMode.StencilReference = 1;
        stencilMode.FrontFace = faceOperation;

        m_PositionalLightPipeline1.GetDepthStencilState()->SetStencilMode(stencilMode);

        m_PositionalLightPipeline1.SetShader(Shader::Type::Vertex, ShaderAssets::DeferredVertexOnly());
    }

    //Pipeline2 for point and spot lights
    {
        m_PositionalLightPipeline2.SetRenderTarget(mainRenderTarget);
        m_PositionalLightPipeline2.GetRasterizerState().SetCullMode(CullMode::Front);
        m_PositionalLightPipeline2.GetRasterizerState().SetDepthClipEnabled(false);

        m_PositionalLightPipeline2.GetBlendState()->SetBlendMode(BlendMode::Additive);

        DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::GreaterOrEqual };
        m_PositionalLightPipeline2.GetDepthStencilState()->SetDepthMode(depthMode);

        StencilMode stencilMode{ true };
        FaceOperation faceOperation;
        faceOperation.StencilFunction = CompareFunction::Equal;
        faceOperation.StencilDepthPass = StencilOperation::Keep;
        stencilMode.StencilReference = 1;
        stencilMode.BackFace = faceOperation;

        m_PositionalLightPipeline2.GetDepthStencilState()->SetStencilMode(stencilMode);
        m_PositionalLightPipeline2.SetShader(Shader::Type::Vertex, ShaderAssets::DeferredPointLight());
        m_PositionalLightPipeline2.SetShader(Shader::Type::Pixel, ShaderAssets::DeferredPointLight());
    }
}

DeferredTechnique::~DeferredTechnique()
{

}


void DeferredTechnique::BindTextures()
{
    LEV_PROFILE_FUNCTION();

    m_DiffuseTexture->Bind(1, Shader::Type::Pixel);
    m_SpecularTexture->Bind(2, Shader::Type::Pixel);
    m_NormalTexture->Bind(3, Shader::Type::Pixel);
    m_DepthTexture->Bind(4, Shader::Type::Pixel);
}

void DeferredTechnique::UnbindTextures()
{
    LEV_PROFILE_FUNCTION();

    m_DiffuseTexture->Unbind(1, Shader::Type::Pixel);
    m_SpecularTexture->Unbind(2, Shader::Type::Pixel);
    m_NormalTexture->Unbind(3, Shader::Type::Pixel);
    m_DepthTexture->Bind(4, Shader::Type::Pixel);
}

void DeferredTechnique::StartOpaquePass()
{
    LEV_PROFILE_FUNCTION();

	m_GBufferRenderTarget->Clear();
    m_GeometryPipeline.Bind();
}

void DeferredTechnique::StartPositionalLightingPass1()
{
    LEV_PROFILE_FUNCTION();

    m_GeometryPipeline.Unbind();
    m_DepthOnlyRenderTarget->Clear(ClearFlags::Stencil, Vector4::Zero, 1, 1);
    m_PositionalLightPipeline1.Bind();
}

void DeferredTechnique::StartPositionalLightingPass2()
{
    LEV_PROFILE_FUNCTION();

    BindTextures();
    m_PositionalLightPipeline1.Unbind();
    m_PositionalLightPipeline2.Bind();
}

void DeferredTechnique::StartDirectionalLightingPass()
{
    LEV_PROFILE_FUNCTION();

    const Ref<Texture> depthStencilBuffer = m_DepthOnlyRenderTarget->GetTexture(AttachmentPoint::DepthStencil);

    if (depthStencilBuffer)
        depthStencilBuffer->Copy(m_DepthTexture);

    BindTextures();
}

void DeferredTechnique::EndLightningPass()
{
    LEV_PROFILE_FUNCTION();

    m_PositionalLightPipeline2.Unbind();
    UnbindTextures();
}
}
