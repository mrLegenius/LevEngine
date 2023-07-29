#include "pch.h"
#include "CascadeShadowMap.h"

#include "RenderSettings.h"

namespace LevEngine
{

CascadeShadowMap::CascadeShadowMap(const uint32_t width, const uint32_t height) : m_Width(width), m_Height(height)
{
	const Texture::TextureFormat textureFormat
	{
        Texture::Components::Depth,
        Texture::Type::Float,
		1,
        0, 0, 0, 0, 32, 0
    };

    m_Texture = Texture::CreateTexture2D(
        RenderSettings::ShadowMapResolution, 
        RenderSettings::ShadowMapResolution, 
        RenderSettings::CascadeCount, 
        textureFormat);

    m_RenderTarget = RenderTarget::Create();
    m_RenderTarget->AttachTexture(AttachmentPoint::Depth, m_Texture);

    m_SamplerState = SamplerState::Create();
    m_SamplerState->SetWrapMode(SamplerState::WrapMode::Border, SamplerState::WrapMode::Border, SamplerState::WrapMode::Border);
    m_SamplerState->SetBorderColor(Color{ 1, 1, 1, 1 });
    m_SamplerState->SetMinLOD(0);
    m_SamplerState->SetMaxLOD(3.402823466e+38f);
    m_SamplerState->SetLODBias(0.0f);
    m_SamplerState->SetMaxAnisotropy(0);
    m_SamplerState->SetCompareMode(SamplerState::CompareMode::CompareRefToTexture);
    m_SamplerState->SetCompareFunction(SamplerState::CompareFunc::LessOrEqual);
    m_SamplerState->SetFilter(SamplerState::MinFilter::Linear, SamplerState::MagFilter::Linear, SamplerState::MipFilter::Linear);

    m_RastState = RasterizerState::Create();
    m_RastState->SetCullMode(CullMode::None);
    m_RastState->SetFrontFaceFillMode(FillMode::Solid);
}

void CascadeShadowMap::Bind(const uint32_t slot) const
{
    m_Texture->Bind(slot);
    m_SamplerState->Bind(slot, Shader::Type::Pixel);
}

void CascadeShadowMap::SetRenderTarget() const
{
    m_RastState->Bind();
    m_RenderTarget->Bind();
    m_RenderTarget->Clear();
}
}
