#pragma once
#include "RenderPass.h"

namespace LevEngine
{
    class PipelineState;

    class OpaquePass final : public RenderPass
    {
    public:
        explicit OpaquePass(const Ref<PipelineState>& pipelineState);

        String PassName() override;
        bool Begin(entt::registry& registry, RenderParams& params) override;
        void Process(entt::registry& registry, RenderParams& params) override;
        void End(entt::registry& registry, RenderParams& params) override;

    private:
        Ref<PipelineState> m_PipelineState;
    };
}
