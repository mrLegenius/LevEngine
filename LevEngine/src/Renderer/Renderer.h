#pragma once

#include "RenderParams.h"
#include "entt/entt.hpp"

namespace LevEngine
{
	class RenderTechnique;
	class PipelineState;
	class RenderTarget;
	class Texture;
	struct Transform;

	class Renderer
{
public:
	static void Init();
	static void Clear();
	static void RecalculateAllTransforms(entt::registry& registry);
	static void LocateCamera(entt::registry& registry, SceneCamera*& mainCamera, Transform*& cameraTransform);
	static RenderParams CreateRenderParams(SceneCamera* mainCamera, Transform* cameraTransform);
	static void Render(entt::registry& registry, SceneCamera* mainCamera, const Transform* cameraTransform);
	static void Render(entt::registry& registry);
	static void Shutdown();
	static void SetViewport(float width, float height);

private:
	inline static Ref<Texture> m_ColorTexture;
	inline static Ref<Texture> m_DepthTexture;
	inline static Ref<Texture> m_AlbedoTexture;
	inline static Ref<Texture> m_MetallicRoughnessAOTexture;
	inline static Ref<Texture> m_NormalTexture;
	
	inline static Ref<RenderTarget> s_DeferredLightsRenderTarget;
	inline static Ref<RenderTarget> s_GBufferRenderTarget;
	inline static Ref<RenderTarget> s_DepthOnlyRenderTarget;
	
	inline static Ref<PipelineState> s_DeferredQuadPipeline;
	inline static Ref<PipelineState> s_GBufferPipeline;
	inline static Ref<PipelineState> s_OpaquePipeline;
	inline static Ref<PipelineState> s_TransparentPipeline;
	inline static Ref<PipelineState> s_DebugPipeline;
	inline static Ref<PipelineState> s_SkyboxPipeline;
	inline static Ref<PipelineState> s_ParticlesPipelineState;
	inline static Ref<PipelineState> m_PositionalLightPipeline1;
	inline static Ref<PipelineState> m_PositionalLightPipeline2;
	
	inline static Ref<RenderTechnique> s_DeferredTechnique;
	inline static Ref<RenderTechnique> s_ForwardTechnique;
};
}
