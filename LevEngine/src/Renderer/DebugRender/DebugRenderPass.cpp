#include "levpch.h"
#include "DebugRenderPass.h"

#include "DebugRender.h"
#include "DebugShape.h"
#include "Assets/EngineAssets.h"
#include "Kernel/Time/Time.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/IndexBuffer.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/VertexBuffer.h"

namespace LevEngine
{
    DebugRenderPass::DebugRenderPass(const Ref<PipelineState>& pipelineState) : m_PipelineState(pipelineState)
    {
        m_ConstantBuffer = ConstantBuffer::Create(sizeof Color);
    }

    String DebugRenderPass::PassName() { return "Debug Render"; }

    bool DebugRenderPass::Begin(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Bind();
        return RenderPass::Begin(registry, params);
    }

    void DebugRenderPass::Process(entt::registry& registry, RenderParams& params)
    {
        AddTemporaryShapesToDrawQueue();

        //TODO: Make debug batching to draw all shapes in one draw call

        ShaderAssets::DebugShape()->Bind();

        auto& shapes = DebugRender::GetRequestedShapes();
        while (!shapes.empty())
        {
            const auto& gizmo = shapes.front();
            m_ConstantBuffer->SetData(&gizmo->GetColor());
            m_ConstantBuffer->Bind(7, ShaderType::Pixel);
            gizmo->Draw();
            shapes.pop();
        }

        auto& vertices = DebugRender::GetVertices();
        auto& indices = DebugRender::GetIndices();
        auto& colors = DebugRender::GetColors();

        if (vertices.size() == 0) return;

        ShaderAssets::DebugLine()->Bind();

        const auto vertexBuffer = VertexBuffer::Create(&vertices[0].x, static_cast<uint32_t>(vertices.size()),
                                                       sizeof Vector3);
        const auto colorBuffer = VertexBuffer::Create(colors[0].Raw(), static_cast<uint32_t>(colors.size()),
                                                      sizeof Color);
        const auto indexBuffer = IndexBuffer::Create(&indices[0], static_cast<uint32_t>(indices.size()));
        vertexBuffer->Bind(0);
        colorBuffer->Bind(1);

        RenderCommand::DrawLineList(indexBuffer);
        DebugRender::Reset();
    }

    void DebugRenderPass::End(entt::registry& registry, RenderParams& params)
    {
        m_PipelineState->Unbind();
    }

    void DebugRenderPass::AddTemporaryShapesToDrawQueue()
    {
        auto& temporaryShapes = DebugRender::m_TemporaryShapes;
        for (int i = temporaryShapes.size() - 1; i >= 0; i--)
        {
            auto& shape = temporaryShapes[i];

            shape.DrawCallback();
            shape.TimeLeft -= Time::GetScaledDeltaTime().GetSeconds();

            if (shape.TimeLeft <= 0.0f)
            {
                temporaryShapes.erase(temporaryShapes.begin() + i);
            }
        }
    }
}
