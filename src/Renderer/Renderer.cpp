#include "pch.h"
#include "Renderer.h"

#include "ParticlePass.h"
#include "RenderCommand.h"
#include "Renderer3D.h"
#include "ShadowMapPass.h"
#include "SkyboxPass.h"
#include "Scene/Entity.h"

namespace LevEngine
{
Ref<ShadowMapPass> Renderer::s_ShadowMapPass;
Ref<SkyboxPass> Renderer::s_SkyboxPass;
Ref<ParticlePass> Renderer::s_ParticlePass;

void Renderer::Init()
{
	RenderCommand::Init();
	Renderer3D::Init();

	s_ShadowMapPass = CreateRef<ShadowMapPass>();
	s_SkyboxPass = CreateRef<SkyboxPass>();

	s_ParticlePass = CreateRef<ParticlePass>();
}

void Renderer::Shutdown()
{
    Renderer3D::Shutdown();
}

void DirectionalLightSystem(entt::registry& registry);
void PointLightsSystem(entt::registry& registry);
void MeshDeferredSystem(entt::registry& registry);
void MeshRenderSystem(entt::registry& registry);

void Renderer::RenderDeferred(entt::registry& registry, RenderParams renderParams)
{
    LEV_PROFILE_FUNCTION();

	{
		LEV_PROFILE_SCOPE("ShadowPass");

		s_ShadowMapPass->Execute(registry, renderParams);
	}

	DirectionalLightSystem(registry);

	//G-Buffer Pass
	{
		LEV_PROFILE_SCOPE("OpaquePass");

		Renderer3D::BeginDeferred(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);

		MeshDeferredSystem(registry);
	}

	//Lightning Pass
	{
		LEV_PROFILE_SCOPE("LightningPass");

		//Start Directional light pipeline
		Renderer3D::BeginDeferredDirLightningSubPass(renderParams.Camera);

		Renderer3D::BeginDeferredPositionalLightningSubPass(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);
		//For each light
		const auto view = registry.group<>(entt::get<Transform, PointLightComponent>);
		for (const auto entity : view)
		{
			auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

			auto model = Matrix::CreateScale(light.Range, light.Range, light.Range)
				* Matrix::CreateFromQuaternion(transform.GetWorldOrientation())
				* Matrix::CreateTranslation(transform.GetWorldPosition());

			auto worldPosition = transform.GetWorldPosition();
			auto positionViewSpace = Vector4::Transform(Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f), renderParams.CameraViewMatrix);
			Renderer3D::AddPointLights(positionViewSpace, worldPosition, light);

			// Start pipeline1
			Renderer3D::BeginDeferredPositionalLightningSubPass1(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);
			// Render lights with sphere
			Renderer3D::RenderSphere(model);

			// Start pipeline2
			Renderer3D::BeginDeferredPositionalLightningSubPass2(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);
			// Render lights with sphere
			Renderer3D::RenderSphere(model);
		}

		//Render skybox
		s_SkyboxPass->Execute(registry, renderParams);

		Renderer3D::EndDeferredLightningPass();

		Renderer3D::m_GBuffer->GetNormalTexture()->Bind(8, Shader::Type::Pixel);
		Renderer3D::m_GBuffer->GetDepthTexture()->Bind(9, Shader::Type::Pixel);
		s_ParticlePass->Process(registry, renderParams);
		Renderer3D::m_GBuffer->GetNormalTexture()->Unbind(8, Shader::Type::Pixel);
		Renderer3D::m_GBuffer->GetDepthTexture()->Unbind(9, Shader::Type::Pixel);
	}
}

void Renderer::RenderForward(entt::registry& registry, RenderParams renderParams)
{
    LEV_PROFILE_FUNCTION();

	DirectionalLightSystem(registry);
	PointLightsSystem(registry);

	{
		LEV_PROFILE_SCOPE("ShadowPass");

		s_ShadowMapPass->Execute(registry, renderParams);
	}

	{
		LEV_PROFILE_SCOPE("RenderPass");
		Renderer3D::BeginScene(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);

		Renderer3D::UpdateLights();

		MeshRenderSystem(registry);

		//Render skybox
		s_SkyboxPass->Execute(registry, renderParams);

		Renderer3D::EndScene();

		s_ParticlePass->Execute(registry, renderParams);
	}
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

		if (camera.isMain)
		{
			using namespace entt::literals;
			registry.ctx().emplace_as<Entity>("mainCameraEntity"_hs, Entity(entt::handle(registry, entity)));
			mainCamera = &camera.camera;
			cameraTransform = &transform;
			return;
		}
	}
}

RenderParams Renderer::CreateRenderParams(SceneCamera* mainCamera, const Transform* cameraTransform)
{
	LEV_PROFILE_FUNCTION();

	const auto cameraViewMatrix = cameraTransform->GetModel().Invert();
	const auto cameraPosition = cameraTransform->GetWorldPosition();
	const auto perspectiveViewProjectionMatrix = cameraViewMatrix * mainCamera->GetPerspectiveProjection();
	return { *mainCamera, cameraPosition, cameraViewMatrix, perspectiveViewProjectionMatrix };
}

void Renderer::Render(entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    SceneCamera* mainCamera = nullptr;
	Transform* cameraTransform = nullptr;

	RecalculateAllTransforms(registry);

	LocateCamera(registry, mainCamera, cameraTransform);

    if (!mainCamera) return;

    const auto renderParams = CreateRenderParams(mainCamera, cameraTransform);

    if constexpr (RenderSettings::DeferredRendering)
	    RenderDeferred(registry, renderParams);
    else
	    RenderForward(registry, renderParams);
}

void DirectionalLightSystem(entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.group<>(entt::get<Transform, DirectionalLightComponent>);
    for (const auto entity : view)
    {
        auto [transform, light] = view.get<Transform, DirectionalLightComponent>(entity);

        Renderer3D::SetDirLight(transform.GetForwardDirection(), light);
    }
}

void PointLightsSystem(entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.group<>(entt::get<Transform, PointLightComponent>);
    for (const auto entity : view)
    {
        auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

        Renderer3D::AddPointLights({}, transform.GetWorldPosition(), light);
    }
}

void MeshDeferredSystem(entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.group<>(entt::get<Transform, MeshRendererComponent>);
    for (const auto entity : view)
    {
        ShaderAssets::GBufferPass()->Bind();
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
        Renderer3D::DrawDeferredMesh(transform.GetModel(), mesh);
    }
}

void MeshRenderSystem(entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.group<>(entt::get<Transform, MeshRendererComponent>);
    for (const auto entity : view)
    {
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
        Renderer3D::DrawOpaqueMesh(transform.GetModel(), mesh);
    }
}
}
