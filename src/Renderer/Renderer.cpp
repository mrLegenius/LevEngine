#include "pch.h"
#include "Renderer.h"

#include "ClearPass.h"
#include "CopyTexturePass.h"
#include "DeferredLightingPass.h"
#include "ParticlePass.h"
#include "RenderCommand.h"
#include "Renderer3D.h"
#include "ShadowMapPass.h"
#include "SkyboxPass.h"
#include "Kernel/Application.h"
#include "Scene/Entity.h"

namespace LevEngine
{
Ref<ShadowMapPass> Renderer::s_ShadowMapPass;
Ref<SkyboxPass> Renderer::s_SkyboxPass;
Ref<ParticlePass> Renderer::s_ParticlePass;

Ref<Texture> Renderer::m_DepthTexture;
Ref<Texture> Renderer::m_DiffuseTexture;
Ref<Texture> Renderer::m_SpecularTexture;
Ref<Texture> Renderer::m_NormalTexture;

Ref<RenderTarget> Renderer::s_GBufferRenderTarget;
Ref<RenderTarget> Renderer::s_DepthOnlyRenderTarget;
Ref<GBufferPass> Renderer::s_GBufferPass;
Ref<PipelineState> Renderer::s_GBufferPipeline;
Ref<PipelineState> Renderer::m_PositionalLightPipeline1;
Ref<PipelineState> Renderer::m_PositionalLightPipeline2;

Ref<RenderTechnique> Renderer::s_DeferredTechnique;

void Renderer::Init()
{
	RenderCommand::Init();
	Renderer3D::Init();

	s_ShadowMapPass = CreateRef<ShadowMapPass>();
	s_SkyboxPass = CreateRef<SkyboxPass>();

	const auto& window = Application::Get().GetWindow();

	const auto mainRenderTarget = window.GetContext()->GetRenderTarget();
	s_DepthOnlyRenderTarget = RenderTarget::Create();
	s_DepthOnlyRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));

	const auto width = window.GetWidth();
	const auto height = window.GetHeight();

	//Depth Texture
	{
		const Texture::TextureFormat depthStencilTextureFormat(
			Texture::Components::DepthStencil,
			Texture::Type::UnsignedNormalized,
			1,
			0, 0, 0, 0, 24, 8);
		m_DepthTexture = Texture::CreateTexture2D(width, height, 1, depthStencilTextureFormat);
	}

	//Diffuse
	{
		const Texture::TextureFormat diffuseTextureFormat(
			Texture::Components::RGBA,
			Texture::Type::UnsignedNormalized,
			1,
			8, 8, 8, 8, 0, 0);
		m_DiffuseTexture = Texture::CreateTexture2D(width, height, 1, diffuseTextureFormat);
	}

	//Specular
	{
		const Texture::TextureFormat specularTextureFormat(
			Texture::Components::RGBA,
			Texture::Type::UnsignedNormalized,
			1,
			8, 8, 8, 8, 0, 0);
		m_SpecularTexture = Texture::CreateTexture2D(width, height, 1, specularTextureFormat);
	}

	//Normal
	{
		const Texture::TextureFormat normalTextureFormat(
			Texture::Components::RGBA,
			Texture::Type::Float,
			1,
			32, 32, 32, 32, 0, 0);
		m_NormalTexture = Texture::CreateTexture2D(width, height, 1, normalTextureFormat);
	}

	s_GBufferRenderTarget = RenderTarget::Create();
	s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color0, mainRenderTarget->GetTexture(AttachmentPoint::Color0));
	s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color1, m_DiffuseTexture);
	s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color2, m_SpecularTexture);
	s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color3, m_NormalTexture);

	s_GBufferRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, m_DepthTexture);

	s_GBufferPipeline = CreateRef<PipelineState>();
	s_GBufferPipeline->SetRenderTarget(s_GBufferRenderTarget);
	s_GBufferPipeline->GetRasterizerState().SetCullMode(CullMode::Back);
	s_GBufferPipeline->SetShader(Shader::Type::Vertex, ShaderAssets::GBufferPass());
	s_GBufferPipeline->SetShader(Shader::Type::Pixel, ShaderAssets::GBufferPass());

	s_GBufferPass = CreateRef<GBufferPass>(s_GBufferPipeline);

	//Pipeline1 for point and spot lights
	{
		m_PositionalLightPipeline1 = CreateRef<PipelineState>();
		m_PositionalLightPipeline1->SetRenderTarget(s_DepthOnlyRenderTarget);
		m_PositionalLightPipeline1->GetRasterizerState().SetCullMode(CullMode::Back);
		m_PositionalLightPipeline1->GetRasterizerState().SetDepthClipEnabled(true);

		const DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::Greater };
		m_PositionalLightPipeline1->GetDepthStencilState()->SetDepthMode(depthMode);

		StencilMode stencilMode{ true };
		FaceOperation faceOperation;
		faceOperation.StencilFunction = CompareFunction::Always;
		faceOperation.StencilDepthPass = StencilOperation::DecrementClamp;
		stencilMode.StencilReference = 1;
		stencilMode.FrontFace = faceOperation;

		m_PositionalLightPipeline1->GetDepthStencilState()->SetStencilMode(stencilMode);

		m_PositionalLightPipeline1->SetShader(Shader::Type::Vertex, ShaderAssets::DeferredVertexOnly());
	}

	//Pipeline2 for point and spot lights
	{
		m_PositionalLightPipeline2 = CreateRef<PipelineState>();
		m_PositionalLightPipeline2->SetRenderTarget(mainRenderTarget);
		m_PositionalLightPipeline2->GetRasterizerState().SetCullMode(CullMode::Front);
		m_PositionalLightPipeline2->GetRasterizerState().SetDepthClipEnabled(false);

		m_PositionalLightPipeline2->GetBlendState()->SetBlendMode(BlendMode::Additive);

		const DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::GreaterOrEqual };
		m_PositionalLightPipeline2->GetDepthStencilState()->SetDepthMode(depthMode);

		StencilMode stencilMode{ true };
		FaceOperation faceOperation;
		faceOperation.StencilFunction = CompareFunction::Equal;
		faceOperation.StencilDepthPass = StencilOperation::Keep;
		stencilMode.StencilReference = 1;
		stencilMode.BackFace = faceOperation;

		m_PositionalLightPipeline2->GetDepthStencilState()->SetStencilMode(stencilMode);
		m_PositionalLightPipeline2->SetShader(Shader::Type::Vertex, ShaderAssets::DeferredPointLight());
		m_PositionalLightPipeline2->SetShader(Shader::Type::Pixel, ShaderAssets::DeferredPointLight());
	}

	s_ParticlePass = CreateRef<ParticlePass>(m_DepthTexture, m_NormalTexture);

	s_DeferredTechnique = CreateRef<RenderTechnique>();
	s_DeferredTechnique->AddPass(CreateRef<ShadowMapPass>());
	s_DeferredTechnique->AddPass(CreateRef<ClearPass>(s_GBufferRenderTarget));
	s_DeferredTechnique->AddPass(CreateRef<GBufferPass>(s_GBufferPipeline));
	s_DeferredTechnique->AddPass(CreateRef<CopyTexturePass>(s_DepthOnlyRenderTarget->GetTexture(AttachmentPoint::DepthStencil), m_DepthTexture));
	s_DeferredTechnique->AddPass(CreateRef<ClearPass>(s_DepthOnlyRenderTarget, ClearFlags::Stencil, Vector4::Zero, 1, 1));
	s_DeferredTechnique->AddPass(CreateRef<DeferredLightingPass>(m_PositionalLightPipeline1, m_PositionalLightPipeline2, m_DiffuseTexture, m_SpecularTexture, m_NormalTexture, m_DepthTexture));
	s_DeferredTechnique->AddPass(CreateRef<SkyboxPass>());
	s_DeferredTechnique->AddPass(CreateRef<ParticlePass>(m_DepthTexture, m_NormalTexture));
}

void Renderer::Shutdown()
{
    Renderer3D::Shutdown();
}

void DirectionalLightSystem(entt::registry& registry);
void PointLightsSystem(entt::registry& registry);
void MeshRenderSystem(entt::registry& registry);

void Renderer::RenderForward(entt::registry& registry, RenderParams renderParams)
{
    LEV_PROFILE_FUNCTION();

	DirectionalLightSystem(registry);
	PointLightsSystem(registry);

	//{
	//	LEV_PROFILE_SCOPE("ShadowPass");

	//	s_ShadowMapPass->Execute(registry, renderParams);
	//}

	//{
	//	LEV_PROFILE_SCOPE("RenderPass");
	//	Renderer3D::BeginScene(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);

	//	Renderer3D::UpdateLights();

	//	MeshRenderSystem(registry);

	//	//Render skybox
	//	s_SkyboxPass->Execute(registry, renderParams);

	//	Renderer3D::EndScene();

	//	s_ParticlePass->Execute(registry, renderParams);
	//}
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
		s_DeferredTechnique->Process(registry, renderParams);
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
