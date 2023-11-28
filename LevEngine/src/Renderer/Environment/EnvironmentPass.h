#pragma once
#include "Renderer/RenderPass.h"

#include "EnvironmentPrecomputePass.h"
#include "EnvironmentRenderPass.h"

namespace LevEngine
{
    class RenderTarget;

    class EnvironmentPass final : public RenderPass
    {
    public:
        explicit EnvironmentPass(Ref<RenderTarget> renderTarget);
        ~EnvironmentPass() override = default;
        
        String PassName() override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void SetViewport(Viewport viewport) override;
        
    private:
        Scope<EnvironmentPrecomputePass> m_PrecomputePass;
        Scope<EnvironmentRenderPass> m_RenderPass;
    };
}

