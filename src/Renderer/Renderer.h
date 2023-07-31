#pragma once
#include "GBufferPass.h"
#include "ParticlePass.h"
#include "RenderParams.h"
#include "ShadowMapPass.h"
#include "SkyboxPass.h"
#include "entt/entt.hpp"

namespace LevEngine
{
class Renderer
{
public:
	static void Init();
	static void RenderForward(entt::registry& registry, RenderParams renderParams);
	static void RecalculateAllTransforms(entt::registry& registry);
	static void LocateCamera(entt::registry& registry, SceneCamera*& mainCamera, Transform*& cameraTransform);
	static RenderParams CreateRenderParams(SceneCamera* mainCamera, const Transform* cameraTransform);
	static void Render(entt::registry& registry);
	static void Shutdown();

private:
	static Ref<ShadowMapPass> s_ShadowMapPass;
	static Ref<SkyboxPass> s_SkyboxPass;
	static Ref<ParticlePass> s_ParticlePass;

	static Ref<Texture> m_DepthTexture;
	static Ref<Texture> m_DiffuseTexture;
	static Ref<Texture> m_SpecularTexture;
	static Ref<Texture> m_NormalTexture;
	static Ref<RenderTarget> s_GBufferRenderTarget;
	static Ref<RenderTarget> s_DepthOnlyRenderTarget;
	static Ref<GBufferPass> s_GBufferPass;
	static Ref<PipelineState> s_GBufferPipeline;
	static Ref<PipelineState> m_PositionalLightPipeline1;
	static Ref<PipelineState> m_PositionalLightPipeline2;

	static Ref<RenderTechnique> s_DeferredTechnique;
};
}
