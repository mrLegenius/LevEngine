#pragma once
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class PipelineState;
    class RenderTarget;

    class VignettePass final : public RenderPass
    {
    public:
        explicit VignettePass(const Ref<RenderTarget>& mainRenderTarget);

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;
        void SetViewport(Viewport viewport) override;

    private:
        Ref<RenderTarget> m_RenderTarget;
        Ref<PipelineState> m_Pipeline;
    };
}
