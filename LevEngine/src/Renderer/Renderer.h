#pragma once

#include "Query.h"
#include "RenderParams.h"
#include "Kernel/Statistic.h"

namespace LevEngine
{
    class LightCollection;
    class Query;
    class Window;
    class RenderTechnique;
    class PipelineState;
    class RenderTarget;
    class Texture;
    struct Transform;

    class Renderer final
    {
    public:
        explicit Renderer(const Window& window);
        ~Renderer();
        
        void Init(const Window& window);
        
        void Clear() const;
        static void RecalculateAllTransforms(entt::registry& registry);
        void LocateCamera(entt::registry& registry, SceneCamera*& mainCamera, Transform*& cameraTransform);
        static RenderParams CreateRenderParams(SceneCamera* mainCamera, Transform* cameraTransform);
        void Render(entt::registry& registry, SceneCamera* mainCamera, const Transform* cameraTransform);
        void Render(entt::registry& registry);
        void SetViewport(float width, float height) const;

        [[nodiscard]] Statistic GetFrameStatistic() const;
        [[nodiscard]] Statistic GetDeferredGeometryStatistic() const;
        [[nodiscard]] Statistic GetDeferredLightingStatistic() const;
        [[nodiscard]] Statistic GetDeferredTransparentStatistic() const;
        [[nodiscard]] Statistic GetEnvironmentStatistic() const;
        [[nodiscard]] Statistic GetPostProcessingStatistic() const;
        [[nodiscard]] Statistic GetParticlesStatistic() const;

    private:
        void ResetStatistics();
        static void SampleQuery(const Ref<Query>& query, Statistic& stat);

        Ref<LightCollection> m_Lights;
        Ref<RenderTarget> m_MainRenderTarget;
        
        Ref<Texture> m_ColorTexture;
        Ref<Texture> m_DepthTexture;
        Ref<Texture> m_AlbedoTexture;
        Ref<Texture> m_MetallicRoughnessAOTexture;
        Ref<Texture> m_NormalTexture;

        Ref<RenderTarget> m_DeferredLightsRenderTarget;
        Ref<RenderTarget> m_GBufferRenderTarget;
        Ref<RenderTarget> m_DepthOnlyRenderTarget;

        Ref<PipelineState> m_DeferredQuadPipeline;
        Ref<PipelineState> m_GBufferPipeline;
        Ref<PipelineState> m_OpaquePipeline;
        Ref<PipelineState> m_TransparentPipeline;
        Ref<PipelineState> m_DebugPipeline;
        Ref<PipelineState> m_PositionalLightPipeline1;
        Ref<PipelineState> m_PositionalLightPipeline2;

        Ref<RenderTechnique> m_DeferredTechnique;
        Ref<RenderTechnique> m_ForwardTechnique;

        Ref<Query> m_FrameQuery;
        Statistic m_FrameStat;

        Ref<Query> m_DeferredGeometryQuery;
        Statistic m_DeferredGeometryStat;

        Ref<Query> m_DeferredLightingQuery;
        Statistic m_DeferredLightingStat;

        Ref<Query> m_DeferredTransparentQuery;
        Statistic m_DeferredTransparentStat;

        Ref<Query> m_EnvironmentQuery;
        Statistic m_EnvironmentStat;

        Ref<Query> m_ParticlesQuery;
        Statistic m_ParticlesStat;
        
        Ref<Query> m_PostProcessingQuery;
        Statistic m_PostProcessingStat;
    };
}
