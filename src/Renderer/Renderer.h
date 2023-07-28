#pragma once
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
	static void RenderDeferred(entt::registry& registry,
	                           RenderParams renderParams);
	static void RenderForward(entt::registry& registry,
	                          RenderParams renderParams);
	static void RecalculateAllTransforms(entt::registry& registry);
	static void LocateCamera(entt::registry& registry, SceneCamera*& mainCamera, Transform*& cameraTransform);
	static RenderParams CreateRenderParams(SceneCamera* mainCamera, const Transform* cameraTransform);
	static void Render(entt::registry& registry);
	static void Shutdown();

private:
	static Ref<ShadowMapPass> s_ShadowMapPass;
	static Ref<SkyboxPass> s_SkyboxPass;
	static Ref<ParticlePass> s_ParticlePass;
};
}
