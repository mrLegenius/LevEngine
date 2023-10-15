#include "levpch.h"
#include "PostProcessingPass.h"

#include "LuminanceAdaptationPass.h"
#include "LuminancePass.h"
#include "TonemappingPass.h"
#include "Kernel/Time/Time.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/RenderSettings.h"

namespace LevEngine
{
    PostProcessingPass::PostProcessingPass(const Ref<RenderTarget>& mainRenderTarget, const Ref<Texture>& colorTexture)
    {
        m_LuminancePass = CreateRef<LuminancePass>(colorTexture);
        m_LuminanceAdaptationPass = CreateRef<LuminanceAdaptationPass>();
        m_TonemappingPass = CreateRef<TonemappingPass>(mainRenderTarget, colorTexture);

        m_ConstantBuffer = ConstantBuffer::Create(sizeof GPUConstants, 8);
    }
    
    void PostProcessingPass::SetViewport(const Viewport viewport)
    {
        m_TonemappingPass->SetViewport(viewport);
    }

    bool PostProcessingPass::Begin(entt::registry& registry, RenderParams& params)
    {
        const GPUConstants data{
            RenderSettings::BloomThreshold,
            RenderSettings::BloomMagnitude,
            RenderSettings::BloomBlurSigma,
            RenderSettings::AdaptationRate,
            Time::GetScaledDeltaTime().GetSeconds(),
            RenderSettings::KeyValue,
        };
        
        m_ConstantBuffer->SetData(&data);
        m_ConstantBuffer->Bind(ShaderType::Pixel);
        
        return RenderPass::Begin(registry, params);
    }

    void PostProcessingPass::Process(entt::registry& registry, RenderParams& params)
    {
        m_LuminancePass->Execute(registry, params);
        m_LuminanceAdaptationPass->SetLuminanceMap(m_LuminancePass->GetLuminanceMap());
        m_LuminanceAdaptationPass->Execute(registry, params);
        m_TonemappingPass->SetLuminanceMap(m_LuminanceAdaptationPass->GetCurrentLuminance());
        m_TonemappingPass->Execute(registry, params);

        m_LuminanceAdaptationPass->SwapCurrentLuminanceMap();
    }

    void PostProcessingPass::End(entt::registry& registry, RenderParams& params)
    {
        RenderPass::End(registry, params);
    }
}

