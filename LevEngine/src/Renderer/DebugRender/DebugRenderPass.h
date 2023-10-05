#pragma once
#include "Renderer/ConstantBuffer.h"
#include "Renderer/PipelineState.h"
#include "Renderer/RenderPass.h"

namespace LevEngine
{
class DebugRenderPass final : public RenderPass
{
public:
    DebugRenderPass(const Ref<PipelineState>& pipelineState);
    
protected:
    bool Begin(entt::registry& registry, RenderParams& params) override;
    void Process(entt::registry& registry, RenderParams& params) override;
    void End(entt::registry& registry, RenderParams& params) override;

private:
    Ref<ConstantBuffer> m_ConstantBuffer;

    Ref<PipelineState> m_PipelineState;
};
}