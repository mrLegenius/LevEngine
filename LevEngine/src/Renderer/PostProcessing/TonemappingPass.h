#pragma once

#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class PipelineState;
    class RenderTarget;
    class Texture;

    class TonemappingPass final : public RenderPass
    {
    public:
        explicit TonemappingPass(const Ref<RenderTarget>& mainRenderTarget, const Ref<Texture>& colorTexture);

        void SetLuminanceMap(const Ref<Texture>& map);
        void SetBloomMap(const Ref<Texture>& map);
    
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

        void SetViewport(Viewport viewport) override;
    
    private:
        Ref<RenderTarget> m_RenderTarget;
        Ref<PipelineState> m_Pipeline;
        Ref<Texture> m_ColorTexture;
        Ref<Texture> m_LuminanceMap;
        Ref<Texture> m_BloomMap;
    };
}
