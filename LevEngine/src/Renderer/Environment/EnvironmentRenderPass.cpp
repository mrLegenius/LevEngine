#include "levpch.h"
#include "EnvironmentRenderPass.h"

#include "AtmosphereConstants.h"
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
    EnvironmentRenderPass::EnvironmentRenderPass(const Ref<RenderTarget>& renderTarget,
                                                 const Ref<AtmosphereConstants>& atmosphere)
        : m_Atmosphere(atmosphere)
        , m_CameraConstantBuffer(ConstantBuffer::Create(sizeof Matrix))
    {
        LEV_PROFILE_SCOPE("Environment Render Pass");

        m_SkyboxPipeline = CreateRef<PipelineState>();
        m_SkyboxPipeline->SetRenderTarget(renderTarget);
        m_SkyboxPipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_SkyboxPipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_SkyboxPipeline->GetDepthStencilState()->SetDepthMode(DepthMode{ false });

        // Same cube, same state -- only the pixel shader differs, so the procedural sky can take
        // over from the cubemap without anything downstream noticing.
        m_AtmospherePipeline = CreateRef<PipelineState>();
        m_AtmospherePipeline->SetRenderTarget(renderTarget);
        m_AtmospherePipeline->GetRasterizerState().SetCullMode(CullMode::None);
        m_AtmospherePipeline->GetRasterizerState().SetDepthClipEnabled(false);
        m_AtmospherePipeline->GetDepthStencilState()->SetDepthMode(DepthMode{ false });
    }

    String EnvironmentRenderPass::PassName() { return "Environment Render"; }

    bool EnvironmentRenderPass::IsAtmosphereActive() const
    {
        return m_Atmosphere && m_Atmosphere->IsActive();
    }

    void EnvironmentRenderPass::SetEnvironmentMap(const Ref<Texture>& environmentMap)
    {
        m_EnvironmentMap = environmentMap;
    }

    bool EnvironmentRenderPass::Begin(entt::registry& registry, RenderParams& params)
    {
        //<--- Reassigned every frame to pick up a hot reloaded shader ---<<
        if (IsAtmosphereActive())
        {
            m_AtmospherePipeline->SetShader(ShaderType::Vertex, EnvironmentShaders::AtmosphereSky());
            m_AtmospherePipeline->SetShader(ShaderType::Pixel, EnvironmentShaders::AtmosphereSky());
            return true;
        }

        if (m_EnvironmentMap == nullptr) return false;

        m_SkyboxPipeline->SetShader(ShaderType::Vertex, EnvironmentShaders::Render());
        m_SkyboxPipeline->SetShader(ShaderType::Pixel, EnvironmentShaders::Render());

        return true;
    }

    void EnvironmentRenderPass::Process(entt::registry& registry, RenderParams& params)
    {
        const auto& pipeline = IsAtmosphereActive() ? m_AtmospherePipeline : m_SkyboxPipeline;

        m_CameraConstantBuffer->SetData(&params.CameraPerspectiveViewProjectionMatrix);
        pipeline->GetShader(ShaderType::Vertex)->GetShaderParameterByName("CameraConstantBuffer").Set(
                   m_CameraConstantBuffer);

        // Both paths sample a cubemap: the imported skybox in one, the raymarched sky in the other.
        pipeline->GetShader(ShaderType::Pixel)->GetShaderParameterByName("Cubemap").Set(m_EnvironmentMap);

        // Only the procedural sky composites a skybox behind itself; the skybox path is already
        // drawing the thing itself.
        if (IsAtmosphereActive())
        {
            pipeline->GetShader(ShaderType::Pixel)
                    ->GetShaderParameterByName("SkyboxCubemap").Set(m_SkyboxCubemap);
        }

        pipeline->Bind();
        Renderer3D::DrawCube(pipeline->GetShader(ShaderType::Vertex));
        pipeline->Unbind();
    }

    void EnvironmentRenderPass::SetViewport(const Viewport viewport)
    {
        m_SkyboxPipeline->GetRasterizerState().SetViewport(viewport);
        m_AtmospherePipeline->GetRasterizerState().SetViewport(viewport);
    }
}
