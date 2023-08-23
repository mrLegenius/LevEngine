#pragma once
#include "OpaquePass.h"
#include "ParticlePass.h"
#include "RenderParams.h"
#include "ShadowMapPass.h"
#include "entt/entt.hpp"

namespace LevEngine
{
class Renderer
{
public:
	static void Init();
	static void RecalculateAllTransforms(entt::registry& registry);
	static void LocateCamera(entt::registry& registry, SceneCamera*& mainCamera, Transform*& cameraTransform);
	static RenderParams CreateRenderParams(SceneCamera* mainCamera, const Transform* cameraTransform);
	static void Render(entt::registry& registry, SceneCamera* mainCamera, const Transform* cameraTransform);
	static void Render(entt::registry& registry);
	static void Shutdown();
	static void SetViewport(float width, float height);

private:
	static Ref<Texture> m_DepthTexture;
	static Ref<Texture> m_DiffuseTexture;
	static Ref<Texture> m_SpecularTexture;
	static Ref<Texture> m_NormalTexture;
	static Ref<RenderTarget> s_GBufferRenderTarget;
	static Ref<RenderTarget> s_DepthOnlyRenderTarget;
	static Ref<OpaquePass> s_GBufferPass;
	static Ref<PipelineState> s_GBufferPipeline;
	static Ref<PipelineState> s_OpaquePipeline;
	static Ref<PipelineState> s_SkyboxPipeline;
	static Ref<PipelineState> s_ParticlesPipelineState;
	static Ref<PipelineState> m_PositionalLightPipeline1;
	static Ref<PipelineState> m_PositionalLightPipeline2;

	static Ref<RenderTechnique> s_DeferredTechnique;
	static Ref<RenderTechnique> s_ForwardTechnique;
};
}
