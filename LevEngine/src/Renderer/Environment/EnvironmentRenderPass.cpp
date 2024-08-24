#include "levpch.h"
#include "EnvironmentRenderPass.h"

#include "EnvironmentShaders.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/DepthStencilState.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
    EnvironmentRenderPass::EnvironmentRenderPass(const Ref<RenderTarget>& renderTarget)
        : m_CameraConstantBuffer(ConstantBuffer::Create(sizeof Matrix))
    {
        LEV_PROFILE_SCOPE("Environment Render Pass");

        m_SkyboxPipeline = CreateRef<PipelineState>();
        m_SkyboxPipeline->SetRenderTarget(renderTarget);
        m_SkyboxPipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_SkyboxPipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_SkyboxPipeline->GetDepthStencilState()->SetDepthMode(DepthMode{ false });
        m_SkyboxPipeline->SetShader(ShaderType::Vertex, EnvironmentShaders::Render());
        m_SkyboxPipeline->SetShader(ShaderType::Pixel, EnvironmentShaders::Render());
    }

    String EnvironmentRenderPass::PassName() { return "Environment Render"; }

    void EnvironmentRenderPass::SetEnvironmentMap(const Ref<Texture>& environmentMap)
    {
        m_EnvironmentMap = environmentMap;
    }

    bool EnvironmentRenderPass::Begin(entt::registry& registry, RenderParams& params) { return m_EnvironmentMap != nullptr; }

    void EnvironmentRenderPass::Process(entt::registry& registry, RenderParams& params)
    {
        m_CameraConstantBuffer->SetData(&params.CameraPerspectiveViewProjectionMatrix);
        m_SkyboxPipeline->GetShader(ShaderType::Vertex)->GetShaderParameterByName("CameraConstantBuffer").Set(
                   m_CameraConstantBuffer);
        
        m_SkyboxPipeline->GetShader(ShaderType::Pixel)->GetShaderParameterByName("Cubemap").Set(m_EnvironmentMap);
        
        m_SkyboxPipeline->Bind();
        Renderer3D::DrawCube(m_SkyboxPipeline->GetShader(ShaderType::Vertex));
        m_SkyboxPipeline->Unbind();
    }

    void EnvironmentRenderPass::SetViewport(const Viewport viewport)
    {
        m_SkyboxPipeline->GetRasterizerState().SetViewport(viewport);
    }
}
