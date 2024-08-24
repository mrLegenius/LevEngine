#include "levpch.h"
#include "Renderer.h"

#include "Pipeline/BlendState.h"
#include "Pipeline/DepthStencilState.h"
#include "Pipeline/PipelineState.h"
#include "Pipeline/RasterizerState.h"
#include "Pipeline/RenderTarget.h"
#include "Pipeline/Texture.h"

#include "Shader/ShaderType.h"

#include "Lighting/LightCollection.h"

#include "Passes/ClearPass.h"
#include "Passes/CopyTexturePass.h"
#include "Passes/DeferredLightingPass.h"
#include "Passes/ShadowMapPass.h"
#include "Passes/OpaquePass.h"
#include "Passes/TransparentPass.h"

#include "Query/BeginQueryPass.h"
#include "Query/EndQueryPass.h"

#include "Particles/ParticlePass.h"

#include "Renderer3D.h"
#include "RenderContext.h"

#include "RenderTechnique.h"

#include "Assets/EngineAssets.h"

#include "DebugRender/DebugRenderPass.h"
#include "Environment/EnvironmentPass.h"
#include "Kernel/Window.h"
#include "PostProcessing/PostProcessingPass.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/Transform/Transform.h"
#include "Kernel/Time/Time.h"
#include "Platform/D3D11/D3D11DeferredContexts.h"
#include "Query/Query.h"

namespace LevEngine
{
    Renderer::Renderer(const Window& window)
        : m_MainRenderTarget(window.GetContext()->GetRenderTarget())
    {
        Init(window);
    }

    Renderer::~Renderer() = default;
    
    void Renderer::Init(const Window& window)
    {
        LEV_PROFILE_FUNCTION();

        Renderer3D::Init();

        m_Lights = CreateRef<LightCollection>();
        
        const auto width = window.GetWidth();
        const auto height = window.GetHeight();

        const auto mainRenderTarget = m_MainRenderTarget;

        {
            LEV_PROFILE_SCOPE("GBuffer color texture creation");

            const Texture::TextureFormat format(
                Texture::Components::RGBA,
                Texture::Type::Float,
                1,
                16, 16, 16, 16, 0, 0);
            m_ColorTexture = Texture::CreateTexture2D(width, height, 1, format);
        }

        {
            LEV_PROFILE_SCOPE("GBuffer depth texture creation");

            const Texture::TextureFormat format(
                Texture::Components::DepthStencil,
                Texture::Type::UnsignedNormalized,
                1,
                0, 0, 0, 0, 24, 8);
            m_DepthTexture = Texture::CreateTexture2D(width, height, 1, format);
        }

        {
            LEV_PROFILE_SCOPE("GBuffer albedo texture creation");

            const Texture::TextureFormat format(
                Texture::Components::RGBA,
                Texture::Type::UnsignedNormalized,
                1,
                8, 8, 8, 8, 0, 0);
            m_AlbedoTexture = Texture::CreateTexture2D(width, height, 1, format);
        }

        {
            LEV_PROFILE_SCOPE("GBuffer normal texture creation");

            const Texture::TextureFormat format(
                Texture::Components::RGBA,
                Texture::Type::Float,
                1,
                32, 32, 32, 32, 0, 0);
            m_NormalTexture = Texture::CreateTexture2D(width, height, 1, format);
        }

        {
            LEV_PROFILE_SCOPE("GBuffer metallic/roughness/ambientOcclusion texture creation");

            const Texture::TextureFormat format(
                Texture::Components::RGBA,
                Texture::Type::UnsignedNormalized,
                1,
                8, 8, 8, 8, 0, 0);
            m_MetallicRoughnessAOTexture = Texture::CreateTexture2D(width, height, 1, format);
        }

        {
            LEV_PROFILE_SCOPE("DepthOnly render target creation");

            m_DepthOnlyRenderTarget = RenderTarget::Create();
            m_DepthOnlyRenderTarget->AttachTexture(AttachmentPoint::DepthStencil,
                                                   mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));
        }

        {
            LEV_PROFILE_SCOPE("Deferred lights render target creation");

            m_DeferredLightsRenderTarget = RenderTarget::Create();
            m_DeferredLightsRenderTarget->AttachTexture(AttachmentPoint::Color0, m_ColorTexture);
            m_DeferredLightsRenderTarget->AttachTexture(AttachmentPoint::DepthStencil,
                                                        mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));
        }

        {
            LEV_PROFILE_SCOPE("Deferred quad pipeline creation");

            m_DeferredQuadPipeline = CreateRef<PipelineState>();
            m_DeferredQuadPipeline->GetRasterizerState().SetCullMode(CullMode::None);
            m_DeferredQuadPipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
            m_DeferredQuadPipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false});
            m_DeferredQuadPipeline->SetShader(ShaderType::Vertex, ShaderAssets::DeferredQuadRender());
            m_DeferredQuadPipeline->SetShader(ShaderType::Pixel, ShaderAssets::DeferredQuadRender());
            m_DeferredQuadPipeline->SetRenderTarget(mainRenderTarget);
        }

        {
            LEV_PROFILE_SCOPE("GBuffer render target creation");

            m_GBufferRenderTarget = RenderTarget::Create();
            m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color0, m_ColorTexture);
            m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color1, m_AlbedoTexture);
            m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color2, m_NormalTexture);
            m_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color3, m_MetallicRoughnessAOTexture);
            m_GBufferRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, m_DepthTexture);
        }

        {
            LEV_PROFILE_SCOPE("GBuffer pipeline creation");

            m_GBufferPipeline = CreateRef<PipelineState>();
            m_GBufferPipeline->SetRenderTarget(m_GBufferRenderTarget);
            m_GBufferPipeline->GetRasterizerState().SetCullMode(CullMode::Back);
            m_GBufferPipeline->SetShader(ShaderType::Vertex, ShaderAssets::GBufferPass());
            m_GBufferPipeline->SetShader(ShaderType::Pixel, ShaderAssets::GBufferPass());
        }

        {
            LEV_PROFILE_SCOPE("Deferred lighting pipeline 1 creation");

            m_PositionalLightPipeline1 = CreateRef<PipelineState>();
            m_PositionalLightPipeline1->SetRenderTarget(m_DeferredLightsRenderTarget);
            m_PositionalLightPipeline1->GetRasterizerState().SetCullMode(CullMode::Back);
            m_PositionalLightPipeline1->GetRasterizerState().SetDepthClipEnabled(true);

            const DepthMode depthMode{true, DepthWrite::Disable, CompareFunction::Greater};
            m_PositionalLightPipeline1->GetDepthStencilState()->SetDepthMode(depthMode);

            StencilMode stencilMode{true};
            FaceOperation faceOperation;
            faceOperation.StencilFunction = CompareFunction::Always;
            faceOperation.StencilDepthPass = StencilOperation::DecrementClamp;
            stencilMode.StencilReference = 1;
            stencilMode.FrontFace = faceOperation;

            m_PositionalLightPipeline1->GetDepthStencilState()->SetStencilMode(stencilMode);

            m_PositionalLightPipeline1->SetShader(ShaderType::Vertex, ShaderAssets::DeferredVertexOnly());
        }

        {
            LEV_PROFILE_SCOPE("Deferred lighting pipeline 2 creation");

            m_PositionalLightPipeline2 = CreateRef<PipelineState>();
            m_PositionalLightPipeline2->SetRenderTarget(m_DeferredLightsRenderTarget);
            m_PositionalLightPipeline2->GetRasterizerState().SetCullMode(CullMode::Front);
            m_PositionalLightPipeline2->GetRasterizerState().SetDepthClipEnabled(false);

            m_PositionalLightPipeline2->GetBlendState()->SetBlendMode(BlendMode::Additive);

            const DepthMode depthMode{true, DepthWrite::Disable, CompareFunction::GreaterOrEqual};
            m_PositionalLightPipeline2->GetDepthStencilState()->SetDepthMode(depthMode);

            StencilMode stencilMode{true};
            FaceOperation faceOperation;
            faceOperation.StencilFunction = CompareFunction::Equal;
            faceOperation.StencilDepthPass = StencilOperation::Keep;
            stencilMode.WriteMask = 0x00;
            stencilMode.StencilReference = 1;
            stencilMode.BackFace = faceOperation;

            m_PositionalLightPipeline2->GetDepthStencilState()->SetStencilMode(stencilMode);
            m_PositionalLightPipeline2->SetShader(ShaderType::Vertex, ShaderAssets::DeferredPointLight());
            m_PositionalLightPipeline2->SetShader(ShaderType::Pixel, ShaderAssets::DeferredPointLight());
        }

        {
            LEV_PROFILE_SCOPE("Forward opaque pipeline creation");

            m_OpaquePipeline = CreateRef<PipelineState>();
            m_OpaquePipeline->GetRasterizerState().SetCullMode(CullMode::Back);
            m_OpaquePipeline->SetShader(ShaderType::Vertex, ShaderAssets::ForwardPBR());
            m_OpaquePipeline->SetShader(ShaderType::Pixel, ShaderAssets::ForwardPBR());
            m_OpaquePipeline->SetRenderTarget(mainRenderTarget);
        }

        {
            LEV_PROFILE_SCOPE("Transparent pipeline creation");

            m_TransparentPipeline = CreateRef<PipelineState>();
            m_TransparentPipeline->SetShader(ShaderType::Vertex, ShaderAssets::ForwardPBR());
            m_TransparentPipeline->SetShader(ShaderType::Pixel, ShaderAssets::ForwardPBR());
            m_TransparentPipeline->GetBlendState()->SetBlendMode(BlendMode::AlphaBlending);
            m_TransparentPipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthWrites);
            m_TransparentPipeline->GetRasterizerState().SetCullMode(CullMode::None);
            m_TransparentPipeline->SetRenderTarget(mainRenderTarget);
        }

        {
            LEV_PROFILE_SCOPE("Debug pipeline creation");

            m_DebugPipeline = CreateRef<PipelineState>();
            m_DebugPipeline->SetRenderTarget(mainRenderTarget);
            m_DebugPipeline->GetRasterizerState().SetAntialiasedLineEnable(true);
            m_DebugPipeline->GetRasterizerState().SetCullMode(CullMode::None);
        }

        {
            LEV_PROFILE_SCOPE("GPU Queries creation");

            constexpr size_t gpuTimersBuffers = 5;
        
            m_FrameQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_ShadowMapQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_EnvironmentQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_DeferredGeometryQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_DeferredLightingQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_DeferredTransparentQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_PostProcessingQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_ParticlesQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
            m_DebugQuery = Query::Create(Query::QueryType::Timer, gpuTimersBuffers);
        }

        {
            LEV_PROFILE_SCOPE("Deferred technique creation");

            m_DeferredTechnique = CreateRef<RenderTechnique>();
            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_ShadowMapQuery));
            m_DeferredTechnique->AddPass(CreateRef<ShadowMapPass>());
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_ShadowMapQuery));
            
            m_DeferredTechnique->AddPass(CreateRef<ClearPass>(mainRenderTarget, "Clear Main Render Target"));
            m_DeferredTechnique->AddPass(CreateRef<ClearPass>(m_GBufferRenderTarget, "Clear G-Buffer"));
            
            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_EnvironmentQuery));
            m_DeferredTechnique->AddPass(CreateRef<EnvironmentPass>(m_GBufferRenderTarget));
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_EnvironmentQuery));

            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_DeferredGeometryQuery));
            m_DeferredTechnique->AddPass(CreateRef<OpaquePass>(m_GBufferPipeline));
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_DeferredGeometryQuery));
            
            m_DeferredTechnique->AddPass(CreateRef<CopyTexturePass>(
                m_DepthOnlyRenderTarget->GetTexture(AttachmentPoint::DepthStencil), m_DepthTexture,
                "Copy Depth Buffer to Texture"));
            m_DeferredTechnique->AddPass(CreateRef<ClearPass>(m_DepthOnlyRenderTarget, "Clear Depth Buffer",
                                                              ClearFlags::Stencil, Vector4::Zero, 1.0f, 1));
            
            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_DeferredLightingQuery));
            m_DeferredTechnique->AddPass(CreateRef<DeferredLightingPass>(
                m_PositionalLightPipeline1, m_PositionalLightPipeline2, m_AlbedoTexture, m_MetallicRoughnessAOTexture,
                m_NormalTexture, m_DepthTexture));
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_DeferredLightingQuery));
            
            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_PostProcessingQuery));
            m_DeferredTechnique->AddPass(CreateRef<PostProcessingPass>(mainRenderTarget, m_ColorTexture));
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_PostProcessingQuery));

            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_DeferredTransparentQuery));
            m_DeferredTechnique->AddPass(CreateRef<TransparentPass>(m_TransparentPipeline));
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_DeferredTransparentQuery));

            m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_DebugQuery));
            m_DeferredTechnique->AddPass(CreateRef<DebugRenderPass>(m_DebugPipeline));
            m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_DebugQuery));
            
            // m_DeferredTechnique->AddPass(CreateRef<BeginQueryPass>(m_ParticlesQuery));
            // m_DeferredTechnique->AddPass(
            //     CreateRef<ParticlePass>(mainRenderTarget, m_DepthTexture, m_NormalTexture));
            // m_DeferredTechnique->AddPass(CreateRef<EndQueryPass>(m_ParticlesQuery));
        }

        {
            LEV_PROFILE_SCOPE("Forward technique creation");

            m_ForwardTechnique = CreateRef<RenderTechnique>();
            m_ForwardTechnique->AddPass(CreateRef<ShadowMapPass>());
            m_ForwardTechnique->AddPass(CreateRef<ClearPass>(mainRenderTarget, "Clear Main Render Target"));
            m_ForwardTechnique->AddPass(CreateRef<EnvironmentPass>(mainRenderTarget));
            m_ForwardTechnique->AddPass(CreateRef<OpaquePass>(m_OpaquePipeline));
            m_ForwardTechnique->AddPass(CreateRef<DebugRenderPass>(m_DebugPipeline));

            //TODO: Fix particle bounce
            m_ForwardTechnique->AddPass(CreateRef<TransparentPass>(m_TransparentPipeline));
            m_ForwardTechnique->AddPass(CreateRef<ParticlePass>(mainRenderTarget,
                                                                mainRenderTarget->GetTexture(
                                                                    AttachmentPoint::DepthStencil), m_NormalTexture));
        }

        SetViewport(static_cast<float>(width), static_cast<float>(height));
    }

    void Renderer::SetViewport(const float width, const float height) const
    {
        static Viewport viewport{};
        if (Math::IsEqual(viewport.width, width) && Math::IsEqual(viewport.height, height)) return;

        viewport = {0.0f, 0.0f, width, height};

        m_PositionalLightPipeline1->GetRasterizerState().SetViewport(viewport);
        m_PositionalLightPipeline2->GetRasterizerState().SetViewport(viewport);
        m_GBufferPipeline->GetRasterizerState().SetViewport(viewport);
        m_OpaquePipeline->GetRasterizerState().SetViewport(viewport);
        m_TransparentPipeline->GetRasterizerState().SetViewport(viewport);
        m_DebugPipeline->GetRasterizerState().SetViewport(viewport);
        m_DeferredQuadPipeline->GetRasterizerState().SetViewport(viewport);

        m_DepthOnlyRenderTarget->Resize(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
        m_GBufferRenderTarget->Resize(static_cast<uint16_t>(width), static_cast<uint16_t>(height));

        m_ForwardTechnique->SetViewport(viewport);
        m_DeferredTechnique->SetViewport(viewport);
    }

    void DirectionalLightSystem(entt::registry& registry);
    void PointLightsSystem(entt::registry& registry, const RenderParams& params);
    void SpotLightsSystem(entt::registry& registry, const RenderParams& params);

    void Renderer::Clear() const
    {
        m_MainRenderTarget->Clear(ClearFlags::All);
    }

    Statistic Renderer::GetFrameStatistic() const { return m_FrameStat; }

    Statistic Renderer::GetShadowMapStatistic() const
    {
        return m_ShadowMapStat;
    }

    Statistic Renderer::GetDeferredGeometryStatistic() const
    {
        return m_DeferredGeometryStat;
    }

    Statistic Renderer::GetDeferredLightingStatistic() const
    {
        return m_DeferredLightingStat;
    }

    Statistic Renderer::GetDeferredTransparentStatistic() const
    {
        return m_DeferredTransparentStat;
    }

    Statistic Renderer::GetEnvironmentStatistic() const
    {
        return m_EnvironmentStat;
    }

    Statistic Renderer::GetPostProcessingStatistic() const
    {
        return m_PostProcessingStat;
    }

    Statistic Renderer::GetParticlesStatistic() const
    {
        return m_ParticlesStat;
    }

    Statistic Renderer::GetDebugStatistic() const
    {
        return m_DebugStat;
    }

    void Renderer::RecalculateAllTransforms(entt::registry& registry)
    {
        LEV_PROFILE_FUNCTION();

        const auto view = registry.view<Transform>();
        for (const auto entity : view)
        {
            auto& transform = view.get<Transform>(entity);
            transform.RecalculateModel();
        }
    }

    void Renderer::LocateCamera(entt::registry& registry, SceneCamera*& mainCamera, Transform*& cameraTransform)
    {
        LEV_PROFILE_FUNCTION();

        const auto group = registry.group<>(entt::get<Transform, CameraComponent>);
        for (const auto entity : group)
        {
            auto [transform, camera] = group.get<Transform, CameraComponent>(entity);

            if (camera.IsMain)
            {
                mainCamera = &camera.Camera;
                cameraTransform = &transform;
                return;
            }
        }
    }

    RenderParams Renderer::CreateRenderParams(SceneCamera* mainCamera, const Transform* cameraTransform)
    {
        LEV_PROFILE_FUNCTION();
        
        const auto cameraPosition = cameraTransform->GetWorldPosition();
        const auto cameraViewMatrix =
            (Matrix::CreateFromQuaternion(cameraTransform->GetWorldRotation())
                * Matrix::CreateTranslation(cameraPosition))
            .Invert();
 
        const auto perspectiveViewProjectionMatrix = cameraViewMatrix * mainCamera->GetPerspectiveProjection();
        return {mainCamera, cameraPosition, cameraViewMatrix, perspectiveViewProjectionMatrix};
    }
    
    void Renderer::Render(entt::registry& registry, SceneCamera* mainCamera, const Transform* cameraTransform)
    {
        LEV_PROFILE_FUNCTION();

        //Multithreading hack for directx 11 and deferred contexts
        if (RenderSettings::RendererAPI == RendererAPI::D3D11)
        {
            D3D11DeferredContexts::UpdateCommandLists();
            D3D11DeferredContexts::ExecuteCommands();
        }

        RecalculateAllTransforms(registry);
        
        if (!mainCamera)
        {
            Clear();
            return;
        }

        static float statResetTimer = 0;
        statResetTimer += Time::GetScaledDeltaTime().GetSeconds();

        if (statResetTimer > 1.0f)
        {
            ResetStatistics();
            statResetTimer -= 1.0f;
        }

        mainCamera->RecalculateFrustum(*cameraTransform);

        const auto renderParams = CreateRenderParams(mainCamera, cameraTransform);

        m_Lights->Prepare(registry, renderParams);

        //TODO: Maybe move to its own pass?
        Renderer3D::SetCameraBuffer(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);

        m_FrameQuery->Begin(Time::GetFrameNumber());
        
        switch (RenderSettings::RenderTechnique)
        {
        case RenderTechniqueType::Forward:
            m_ForwardTechnique->Process(registry, renderParams);
            break;
        case RenderTechniqueType::Deferred:
            m_DeferredTechnique->Process(registry, renderParams);
            break;
        case RenderTechniqueType::ForwardPlus:
            LEV_NOT_IMPLEMENTED
            break;
        }
        
        m_FrameQuery->End(Time::GetFrameNumber());
        
        SampleQuery(m_FrameQuery, m_FrameStat);
        SampleQuery(m_ShadowMapQuery, m_ShadowMapStat);
        SampleQuery(m_DeferredGeometryQuery, m_DeferredGeometryStat);
        SampleQuery(m_DeferredLightingQuery, m_DeferredLightingStat);
        SampleQuery(m_DeferredTransparentQuery, m_DeferredTransparentStat);
        SampleQuery(m_EnvironmentQuery, m_EnvironmentStat);
        SampleQuery(m_PostProcessingQuery, m_PostProcessingStat);
        SampleQuery(m_ParticlesQuery, m_ParticlesStat);
        SampleQuery(m_DebugQuery, m_DebugStat);
    }

    void Renderer::ResetStatistics()
    {
        m_FrameStat.Reset();

        m_ShadowMapStat.Reset();
        m_DeferredGeometryStat.Reset();
        m_DeferredLightingStat.Reset();
        m_DeferredTransparentStat.Reset();
        
        m_EnvironmentStat.Reset();
        m_PostProcessingStat.Reset();
        m_ParticlesStat.Reset();
        m_DebugStat.Reset();
    }

    void Renderer::SampleQuery(const Ref<Query>& query, Statistic& stat)
    {
        // Retrieve GPU timer results.
        // Don't retrieve the immediate query result, but from the previous frame.
        // Checking previous frame counters will alleviate GPU stalls.
        
        const auto queryResult = query->GetQueryResult(Time::GetFrameNumber() - (query->GetBufferCount() - 1));
        if (queryResult.IsValid)    
        {
            stat.Sample(queryResult.ElapsedTime * 1000.0);
        }
    }

    void Renderer::Render(entt::registry& registry)
    {
        LEV_PROFILE_FUNCTION();

        SceneCamera* mainCamera = nullptr;
        Transform* cameraTransform = nullptr;

        LocateCamera(registry, mainCamera, cameraTransform);

        Render(registry, mainCamera, cameraTransform);
    }
}
