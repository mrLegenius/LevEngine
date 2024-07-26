#pragma once
#include "Renderer/Passes/RenderPass.h"


namespace LevEngine
{
    class ConstantBuffer;
    class PipelineState;

    class DebugRenderPass final : public RenderPass
    {
    public:
        DebugRenderPass(const Ref<PipelineState>& pipelineState);

    protected:
        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        static void AddTemporaryShapesToDrawQueue();
        Ref<ConstantBuffer> m_ConstantBuffer;
        Ref<PipelineState> m_PipelineState;
    };
}
