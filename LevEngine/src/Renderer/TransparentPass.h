#pragma once
#include "RenderPass.h"

namespace LevEngine
{
    class PipelineState;

    class TransparentPass final : public RenderPass
{
public:
    explicit TransparentPass(const Ref<PipelineState>& pipelineState);

protected:
    bool Begin(entt::registry& registry, RenderParams& params) override;
    void Process(entt::registry& registry, RenderParams& params) override;
    void End(entt::registry& registry, RenderParams& params) override;

private:
    Ref<PipelineState> m_PipelineState;
};
}
