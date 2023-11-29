#pragma once
#include "Renderer/RenderPass.h"

#include "Math/Color.h"
#include "Math/Vector2.h"

namespace LevEngine
{
    class RenderTarget;
    class Texture;
    class ConstantBuffer;
    class SamplerState;
    
    class LuminanceAdaptationPass;
    class LuminancePass;
    class TonemappingPass;
    class BloomPass;
    class VignettePass;
    
    class PostProcessingPass final : public RenderPass
    {
    public:
        PostProcessingPass(const Ref<RenderTarget>& mainRenderTarget, const Ref<Texture>& colorTexture);
        
        void SetViewport(Viewport viewport) override;
        
    protected:
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
    
    private:
        struct alignas(16) GPUConstants
        {
            float BloomThreshold;
            float BloomMagnitude;
            float BloomBlurSigma;
            float Tau;
            
            float TimeDelta;
            float KeyValue;
            float MinExposure;
            float MaxExposure;

            Color VignetteColor;

            Vector2 VignetteCenter;
            float VignetteRadius;
            float VignetteSoftness;
            
            float VignetteIntensity;
        };
        
        Ref<LuminancePass> m_LuminancePass;
        Ref<LuminanceAdaptationPass> m_LuminanceAdaptationPass;
        Ref<TonemappingPass> m_TonemappingPass;
        Ref<BloomPass> m_BloomPass;
        Ref<VignettePass> m_VignettePass;

        Ref<ConstantBuffer> m_ConstantBuffer;
        Ref<SamplerState> m_LinearSampler;
        Ref<SamplerState> m_PointSampler;
    };
}
