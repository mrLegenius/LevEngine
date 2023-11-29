#pragma once

#include "RenderParams.h"

namespace LevEngine
{
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
        void Render(entt::registry& registry, SceneCamera* mainCamera, const Transform* cameraTransform) const;
        void Render(entt::registry& registry);
        void SetViewport(float width, float height) const;

    private:
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
        Ref<PipelineState> m_ParticlesPipelineState;
        Ref<PipelineState> m_PositionalLightPipeline1;
        Ref<PipelineState> m_PositionalLightPipeline2;

        Ref<RenderTechnique> m_DeferredTechnique;
        Ref<RenderTechnique> m_ForwardTechnique;
    };
}
