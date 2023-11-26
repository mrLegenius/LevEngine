#include "levpch.h"
#include "CascadeShadowMap.h"
#include "RasterizerState.h"
#include "RenderSettings.h"
#include "RenderTarget.h"
#include "SamplerState.h"

namespace LevEngine
{

CascadeShadowMap::CascadeShadowMap(const uint32_t width, const uint32_t height)
{
	const Texture::TextureFormat textureFormat
	{
        Texture::Components::Depth,
        Texture::Type::Float,
		1,
        0, 0, 0, 0, 32, 0
    };

    m_Texture = Texture::CreateTexture2D(
        width,
        height,
        RenderSettings::CascadeCount,
        textureFormat);

    m_RenderTarget = RenderTarget::Create();
    m_RenderTarget->AttachTexture(AttachmentPoint::Depth, m_Texture);

    auto sampler = SamplerState::Create();
    sampler->SetWrapMode(SamplerState::WrapMode::Border, SamplerState::WrapMode::Border, SamplerState::WrapMode::Border);
    sampler->SetBorderColor(Color{ 1, 1, 1, 1 });
    sampler->SetMinLOD(0);
    sampler->SetMaxLOD(FLT_MAX);
    sampler->SetLODBias(0.0f);
    sampler->SetMaxAnisotropy(0);
    sampler->SetCompareMode(SamplerState::CompareMode::CompareRefToTexture);
    sampler->SetCompareFunction(SamplerState::CompareFunc::LessOrEqual);
    sampler->SetFilter(SamplerState::MinFilter::Linear, SamplerState::MagFilter::Linear, SamplerState::MipFilter::Linear);

    m_Texture->AttachSampler(sampler);

    m_RastState = RasterizerState::Create();
    m_RastState->SetCullMode(CullMode::None);
    m_RastState->SetFrontFaceFillMode(FillMode::Solid);
    m_RastState->SetViewport({0, 0, RenderSettings::ShadowMapResolution , RenderSettings::ShadowMapResolution });
}

void CascadeShadowMap::Bind(const uint32_t slot) const
{
    m_Texture->Bind(slot, ShaderType::Pixel);
}

void CascadeShadowMap::SetRenderTarget() const
{
    m_RastState->Bind();
    m_RenderTarget->Bind();
    m_RenderTarget->Clear();
}

void CascadeShadowMap::ResetRenderTarget() const
{
    m_RenderTarget->Unbind();
}
}
