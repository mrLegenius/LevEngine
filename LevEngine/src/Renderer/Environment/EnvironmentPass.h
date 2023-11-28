#pragma once
#include "Renderer/RenderPass.h"

namespace LevEngine
{
    class RenderTarget;
    class EnvironmentPrecomputePass;
    class EnvironmentRenderPass;

    class EnvironmentPass final : public RenderPass
    {
    public:
        explicit EnvironmentPass(Ref<RenderTarget> renderTarget);
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void SetViewport(Viewport viewport) override;
        
    private:
        Scope<EnvironmentPrecomputePass> m_PrecomputePass;
        Scope<EnvironmentRenderPass> m_RenderPass;
    };
}

