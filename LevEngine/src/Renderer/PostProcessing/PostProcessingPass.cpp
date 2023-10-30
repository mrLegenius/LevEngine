#include "levpch.h"
#include "PostProcessingPass.h"

#include "BloomPass.h"
#include "LuminanceAdaptationPass.h"
#include "LuminancePass.h"
#include "TonemappingPass.h"
#include "VignettePass.h"
#include "Kernel/Time/Time.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/SamplerState.h"

namespace LevEngine
{
    PostProcessingPass::PostProcessingPass(const Ref<RenderTarget>& mainRenderTarget, const Ref<Texture>& colorTexture)
    {
        m_LuminancePass = CreateRef<LuminancePass>(colorTexture);
        m_LuminanceAdaptationPass = CreateRef<LuminanceAdaptationPass>();
        m_TonemappingPass = CreateRef<TonemappingPass>(mainRenderTarget, colorTexture);
        m_BloomPass = CreateRef<BloomPass>(colorTexture);
        m_VignettePass = CreateRef<VignettePass>(mainRenderTarget);

        m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUConstants, 8);

        m_LinearSampler = SamplerState::Create();
        m_LinearSampler->SetFilter(SamplerState::MinFilter::Linear, SamplerState::MagFilter::Linear, SamplerState::MipFilter::Linear);
        m_LinearSampler->SetWrapMode(SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp);
        
        m_PointSampler = SamplerState::Create();
        m_PointSampler->SetFilter(SamplerState::MinFilter::Nearest, SamplerState::MagFilter::Nearest, SamplerState::MipFilter::Nearest);
        m_PointSampler->SetWrapMode(SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp);
    }
    
    void PostProcessingPass::SetViewport(const Viewport viewport)
    {
        m_TonemappingPass->SetViewport(viewport);
        m_BloomPass->SetViewport(viewport);
        m_VignettePass->SetViewport(viewport);
    }

    bool PostProcessingPass::Begin(entt::registry& registry, RenderParams& params)
    {
        const GPUConstants data{
            RenderSettings::BloomThreshold,
            RenderSettings::BloomMagnitude,
            RenderSettings::BloomBlurSigma,
            RenderSettings::AdaptationRate,
            Time::GetScaledDeltaTime().GetSeconds(),
            RenderSettings::IsEyeAdaptationEnabled ? RenderSettings::KeyValue : RenderSettings::ManualExposure,
            RenderSettings::IsEyeAdaptationEnabled ? RenderSettings::MinExposure : 0,
            RenderSettings::IsEyeAdaptationEnabled ? RenderSettings::MaxExposure : RenderSettings::ManualExposure,
            RenderSettings::VignetteColor,
            RenderSettings::VignetteCenter,
            RenderSettings::VignetteRadius,
            RenderSettings::VignetteSoftness,
            RenderSettings::VignetteIntensity,
        };
        
        m_ConstantBuffer->SetData(&data);
        m_ConstantBuffer->Bind(ShaderType::Pixel);

        m_LinearSampler->Bind(0, ShaderType::Pixel);
        m_PointSampler->Bind(1, ShaderType::Pixel);
        
        return RenderPass::Begin(registry, params);
    }

    void PostProcessingPass::Process(entt::registry& registry, RenderParams& params)
    {
        m_LuminancePass->Execute(registry, params);

        if (RenderSettings::IsEyeAdaptationEnabled)
        {
            m_LuminanceAdaptationPass->SetLuminanceMap(m_LuminancePass->GetLuminanceMap());
            m_LuminanceAdaptationPass->Execute(registry, params);
        }
        
        if (RenderSettings::IsBloomEnabled)
        {
            m_BloomPass->SetLuminanceMap(m_LuminanceAdaptationPass->GetCurrentLuminance());
            m_BloomPass->Execute(registry, params);
        }

        const auto luminanceMap = RenderSettings::IsEyeAdaptationEnabled
            ? m_LuminanceAdaptationPass->GetCurrentLuminance()
            : TextureLibrary::GetWhiteTexture();
        m_TonemappingPass->SetLuminanceMap(luminanceMap);
        
        const auto bloomMap = RenderSettings::IsBloomEnabled
            ? m_BloomPass->GetBloomMap()
            : TextureLibrary::GetBlackTexture();
        m_TonemappingPass->SetBloomMap(bloomMap);
        
        m_TonemappingPass->Execute(registry, params);
        
        m_VignettePass->Execute(registry, params);
        
        if (RenderSettings::IsEyeAdaptationEnabled)
        {
            m_LuminanceAdaptationPass->SwapCurrentLuminanceMap();
        }
    }

    void PostProcessingPass::End(entt::registry& registry, RenderParams& params)
    {
        m_LinearSampler->Unbind(0, ShaderType::Pixel);
        m_PointSampler->Unbind(1, ShaderType::Pixel);
        
        RenderPass::End(registry, params);
    }
}

