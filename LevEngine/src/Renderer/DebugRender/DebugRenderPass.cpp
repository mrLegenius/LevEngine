#include "levpch.h"
#include "DebugRenderPass.h"

#include "DebugRender.h"

namespace LevEngine
{
DebugRenderPass::DebugRenderPass(const Ref<PipelineState>& pipelineState) : m_PipelineState(pipelineState)
{
    m_ConstantBuffer = ConstantBuffer::Create(sizeof Color);
}

bool DebugRenderPass::Begin(entt::registry& registry, RenderParams& params)
{
    m_PipelineState->Bind();
    return RenderPass::Begin(registry, params);
}

void DebugRenderPass::Process(entt::registry& registry, RenderParams& params)
{
    //TODO: Make debug batching to draw all shapes in one draw call
    
    auto& shapes = DebugRender::GetRequestedShapes();
    while (!shapes.empty())
    {
        const auto& gizmo = shapes.front();
        m_ConstantBuffer->SetData(&gizmo->GetColor());
        m_ConstantBuffer->Bind(2, ShaderType::Pixel);
        gizmo->Draw();
        shapes.pop();
    }
}

void DebugRenderPass::End(entt::registry& registry, RenderParams& params)
{
    m_PipelineState->Unbind();
}
}
