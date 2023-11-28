#include "levpch.h"
#include "EnvironmentRenderPass.h"

#include "Renderer/DepthStencilState.h"
#include "Renderer/PipelineState.h"
#include "Renderer/RasterizerState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"

namespace LevEngine
{
    struct alignas(16) CameraData
    {
        Matrix View;
        Matrix ViewProjection;
        Vector3 Position;
    };

    EnvironmentRenderPass::EnvironmentRenderPass(const Ref<RenderTarget>& renderTarget)
        : m_CameraConstantBuffer(ConstantBuffer::Create(sizeof CameraData))
    {
        LEV_PROFILE_SCOPE("Environment Render Pass");

        m_SkyboxPipeline = CreateRef<PipelineState>();
        m_SkyboxPipeline->SetRenderTarget(renderTarget);
        m_SkyboxPipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_SkyboxPipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_SkyboxPipeline->GetDepthStencilState()->SetDepthMode(DepthMode{ false });
        m_SkyboxPipeline->SetShader(ShaderType::Vertex, ShaderAssets::Skybox());
        m_SkyboxPipeline->SetShader(ShaderType::Pixel, ShaderAssets::Skybox());
    }

    String EnvironmentRenderPass::PassName() { return "Environment Render"; }

    void EnvironmentRenderPass::SetEnvironmentMap(const Ref<Texture>& environmentMap)
    {
        m_EnvironmentMap = environmentMap;
    }

    bool EnvironmentRenderPass::Begin(entt::registry& registry, RenderParams& params) { return m_EnvironmentMap != nullptr; }

    void EnvironmentRenderPass::Process(entt::registry& registry, RenderParams& params)
    {
        const CameraData skyboxCameraData {
            Matrix::Identity, params.CameraPerspectiveViewProjectionMatrix, Vector3::Zero
        };
        m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);

        m_SkyboxPipeline->GetShader(ShaderType::Pixel)->GetShaderParameterByName("Cubemap").Set(m_EnvironmentMap);
        m_SkyboxPipeline->GetShader(ShaderType::Vertex)->GetShaderParameterByName("CameraConstantBuffer").Set(
            m_CameraConstantBuffer);
        m_SkyboxPipeline->Bind();

        Renderer3D::DrawCube(m_SkyboxPipeline->GetShader(ShaderType::Vertex));

        m_SkyboxPipeline->Unbind();
    }

    void EnvironmentRenderPass::SetViewport(const Viewport viewport)
    {
        m_SkyboxPipeline->GetRasterizerState().SetViewport(viewport);
    }
}
