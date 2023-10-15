#pragma once
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class RenderTarget;
    class Texture;
    class LuminanceAdaptationPass;
    class LuminancePass;
    class TonemappingPass;
    class ConstantBuffer;;
    
    class PostProcessingPass final : public RenderPass
    {
    public:
        PostProcessingPass(const Ref<RenderTarget>& mainRenderTarget, const Ref<Texture>& colorTexture);
        
        void SetViewport(Viewport viewport) override;
        
    protected:
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
        };
        
        Ref<LuminancePass> m_LuminancePass;
        Ref<LuminanceAdaptationPass> m_LuminanceAdaptationPass;
        Ref<TonemappingPass> m_TonemappingPass;

        Ref<ConstantBuffer> m_ConstantBuffer;
    };
}
