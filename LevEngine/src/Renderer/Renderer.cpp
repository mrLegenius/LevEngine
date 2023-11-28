#include "levpch.h"
#include "Renderer.h"

#include "BlendState.h"
#include "ClearPass.h"
#include "CopyTexturePass.h"
#include "DeferredLightingPass.h"
#include "EnvironmentPass.h"
#include "DepthStencilState.h"
#include "OpaquePass.h"
#include "ParticlePass.h"
#include "QuadRenderPass.h"
#include "RasterizerState.h"
#include "Renderer3D.h"
#include "RendererContext.h"
#include "RenderTarget.h"
#include "RenderTechnique.h"
#include "ShadowMapPass.h"
#include "SkyboxPass.h"
#include "TransparentPass.h"
#include "DebugRender/DebugRenderPass.h"
#include "Kernel/Application.h"
#include "PostProcessing/LuminanceAdaptationPass.h"
#include "PostProcessing/LuminancePass.h"
#include "PostProcessing/PostProcessingPass.h"
#include "Scene/Entity.h"
#include "Scene/Components/Camera/Camera.h"

namespace LevEngine
{
void Renderer::Init()
{
	LEV_PROFILE_FUNCTION();

	Renderer3D::Init();

	const auto& window = Application::Get().GetWindow();
	const auto width = window.GetWidth();
	const auto height = window.GetHeight();

	const auto mainRenderTarget = window.GetContext()->GetRenderTarget();

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

		s_DepthOnlyRenderTarget = RenderTarget::Create();
		s_DepthOnlyRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));
	}

	{
		LEV_PROFILE_SCOPE("Deferred lights render target creation");

		s_DeferredLightsRenderTarget = RenderTarget::Create();
		s_DeferredLightsRenderTarget->AttachTexture(AttachmentPoint::Color0, m_ColorTexture);
		s_DeferredLightsRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil));
	}

	{
		LEV_PROFILE_SCOPE("Deferred quad pipeline creation");

		s_DeferredQuadPipeline = CreateRef<PipelineState>();
		s_DeferredQuadPipeline->GetRasterizerState().SetCullMode(CullMode::None);
		s_DeferredQuadPipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthTesting);
		s_DeferredQuadPipeline->GetDepthStencilState()->SetStencilMode(StencilMode{false });
		s_DeferredQuadPipeline->SetShader(ShaderType::Vertex, ShaderAssets::DeferredQuadRender());
		s_DeferredQuadPipeline->SetShader(ShaderType::Pixel, ShaderAssets::DeferredQuadRender());
		s_DeferredQuadPipeline->SetRenderTarget(mainRenderTarget);
	}

	{
		LEV_PROFILE_SCOPE("GBuffer render target creation");

		s_GBufferRenderTarget = RenderTarget::Create();
		s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color0, m_ColorTexture);
		s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color1, m_AlbedoTexture);
		s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color2, m_NormalTexture);
		s_GBufferRenderTarget->AttachTexture(AttachmentPoint::Color3, m_MetallicRoughnessAOTexture);
		s_GBufferRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, m_DepthTexture);
	}

	{
		LEV_PROFILE_SCOPE("GBuffer pipeline creation");

		s_GBufferPipeline = CreateRef<PipelineState>();
		s_GBufferPipeline->SetRenderTarget(s_GBufferRenderTarget);
		s_GBufferPipeline->GetRasterizerState().SetCullMode(CullMode::Back);
		s_GBufferPipeline->SetShader(ShaderType::Vertex, ShaderAssets::GBufferPass());
		s_GBufferPipeline->SetShader(ShaderType::Pixel, ShaderAssets::GBufferPass());
	}

	{
		LEV_PROFILE_SCOPE("Deferred lighting pipeline 1 creation");

		m_PositionalLightPipeline1 = CreateRef<PipelineState>();
		m_PositionalLightPipeline1->SetRenderTarget(s_DeferredLightsRenderTarget);
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

		m_PositionalLightPipeline1->SetShader(ShaderType::Vertex, ShaderAssets::DeferredVertexOnly());
	}

	{
		LEV_PROFILE_SCOPE("Deferred lighting pipeline 2 creation");

		m_PositionalLightPipeline2 = CreateRef<PipelineState>();
		m_PositionalLightPipeline2->SetRenderTarget(s_DeferredLightsRenderTarget);
		m_PositionalLightPipeline2->GetRasterizerState().SetCullMode(CullMode::Front);
		m_PositionalLightPipeline2->GetRasterizerState().SetDepthClipEnabled(false);

		m_PositionalLightPipeline2->GetBlendState()->SetBlendMode(BlendMode::Additive);

		const DepthMode depthMode{ true, DepthWrite::Disable, CompareFunction::GreaterOrEqual };
		m_PositionalLightPipeline2->GetDepthStencilState()->SetDepthMode(depthMode);

		StencilMode stencilMode{ true };
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

		s_OpaquePipeline = CreateRef<PipelineState>();
		s_OpaquePipeline->GetRasterizerState().SetCullMode(CullMode::Back);
		s_OpaquePipeline->SetShader(ShaderType::Vertex, ShaderAssets::ForwardPBR());
		s_OpaquePipeline->SetShader(ShaderType::Pixel, ShaderAssets::ForwardPBR());
		s_OpaquePipeline->SetRenderTarget(mainRenderTarget);
	}

	{
		LEV_PROFILE_SCOPE("Transparent pipeline creation");

		s_TransparentPipeline = CreateRef<PipelineState>();
		s_TransparentPipeline->SetShader(ShaderType::Vertex, ShaderAssets::ForwardPBR());
		s_TransparentPipeline->SetShader(ShaderType::Pixel, ShaderAssets::ForwardPBR());
		s_TransparentPipeline->GetBlendState()->SetBlendMode(BlendMode::AlphaBlending);
		s_TransparentPipeline->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthWrites);
		s_TransparentPipeline->GetRasterizerState().SetCullMode(CullMode::None);
		s_TransparentPipeline->SetRenderTarget(mainRenderTarget);
	}

	{
		LEV_PROFILE_SCOPE("Debug pipeline creation");

		s_DebugPipeline = CreateRef<PipelineState>();
		s_DebugPipeline->SetShader(ShaderType::Vertex, ShaderAssets::Debug());
		s_DebugPipeline->SetShader(ShaderType::Pixel, ShaderAssets::Debug());
		s_DebugPipeline->SetRenderTarget(mainRenderTarget);
		s_DebugPipeline->GetRasterizerState().SetAntialiasedLineEnable(true);
		s_DebugPipeline->GetRasterizerState().SetCullMode(CullMode::None);
	}

	{
		LEV_PROFILE_SCOPE("Skybox pipeline creation");

		s_SkyboxPipeline = CreateRef<PipelineState>();
		s_SkyboxPipeline->SetRenderTarget(s_GBufferRenderTarget);
		s_SkyboxPipeline->GetRasterizerState().SetCullMode(CullMode::None);
		s_SkyboxPipeline->GetRasterizerState().SetDepthClipEnabled(false);
		s_SkyboxPipeline->GetDepthStencilState()->SetDepthMode(DepthMode{ true, DepthWrite::Enable, CompareFunction::LessOrEqual });
		s_SkyboxPipeline->SetShader(ShaderType::Vertex, ShaderAssets::Skybox());
		s_SkyboxPipeline->SetShader(ShaderType::Pixel, ShaderAssets::Skybox());
	}

	{
		LEV_PROFILE_SCOPE("Particles pipeline creation");

		s_ParticlesPipelineState = CreateRef<PipelineState>();
		s_ParticlesPipelineState->GetBlendState()->SetBlendMode(BlendMode::AlphaBlending);
		s_ParticlesPipelineState->GetDepthStencilState()->SetDepthMode(DepthMode::DisableDepthWrites);
		s_ParticlesPipelineState->GetRasterizerState().SetCullMode(CullMode::None);
		s_ParticlesPipelineState->SetRenderTarget(mainRenderTarget);
	}

	auto environmentPass = CreateRef<EnvironmentPass>();
	
	{
		LEV_PROFILE_SCOPE("Deferred technique creation");

		s_DeferredTechnique = CreateRef<RenderTechnique>();
		s_DeferredTechnique->AddPass(CreateRef<ShadowMapPass>());
		s_DeferredTechnique->AddPass(CreateRef<ClearPass>(mainRenderTarget, "Clear Main Render Target"));
		s_DeferredTechnique->AddPass(CreateRef<ClearPass>(s_GBufferRenderTarget, "Clear G-Buffer"));
		s_DeferredTechnique->AddPass(CreateRef<OpaquePass>(s_GBufferPipeline));
		s_DeferredTechnique->AddPass(environmentPass);
		s_DeferredTechnique->AddPass(CreateRef<CopyTexturePass>(s_DepthOnlyRenderTarget->GetTexture(AttachmentPoint::DepthStencil), m_DepthTexture, "Copy Depth Buffer to Texture"));
		s_DeferredTechnique->AddPass(CreateRef<ClearPass>(s_DepthOnlyRenderTarget, "Clear Depth Buffer", ClearFlags::Stencil, Vector4::Zero, 1.0f, 1));
		s_DeferredTechnique->AddPass(CreateRef<DeferredLightingPass>(m_PositionalLightPipeline1, m_PositionalLightPipeline2, m_AlbedoTexture, m_MetallicRoughnessAOTexture, m_NormalTexture, m_DepthTexture));
		s_DeferredTechnique->AddPass(CreateRef<SkyboxPass>(s_SkyboxPipeline, environmentPass));
		s_DeferredTechnique->AddPass(CreateRef<PostProcessingPass>(mainRenderTarget, m_ColorTexture));
		s_DeferredTechnique->AddPass(CreateRef<DebugRenderPass>(s_DebugPipeline));
		s_DeferredTechnique->AddPass(CreateRef<TransparentPass>(s_TransparentPipeline));
		s_DeferredTechnique->AddPass(CreateRef<ParticlePass>(s_ParticlesPipelineState, m_DepthTexture, m_NormalTexture));
	}
	
	{
		LEV_PROFILE_SCOPE("Forward technique creation");

		s_ForwardTechnique = CreateRef<RenderTechnique>();
		s_ForwardTechnique->AddPass(CreateRef<ShadowMapPass>());
		s_ForwardTechnique->AddPass(CreateRef<ClearPass>(mainRenderTarget, "Clear Main Render Target"));
		s_ForwardTechnique->AddPass(environmentPass);
		s_ForwardTechnique->AddPass(CreateRef<OpaquePass>(s_OpaquePipeline));
		s_ForwardTechnique->AddPass(CreateRef<DebugRenderPass>(s_DebugPipeline));
		s_ForwardTechnique->AddPass(CreateRef<SkyboxPass>(s_SkyboxPipeline, environmentPass));
		//TODO: Fix particle bounce
		s_ForwardTechnique->AddPass(CreateRef<TransparentPass>(s_TransparentPipeline));
		s_ForwardTechnique->AddPass(CreateRef<ParticlePass>(s_ParticlesPipelineState, mainRenderTarget->GetTexture(AttachmentPoint::DepthStencil), m_NormalTexture));
	}

	SetViewport(static_cast<float>(width), static_cast<float>(height));
}

void Renderer::Shutdown()
{

}

void Renderer::SetViewport(const float width, const float height)
{
	static Viewport viewport{};
	if (Math::IsEqual(viewport.width, width) && Math::IsEqual(viewport.height, height)) return;

	viewport = { 0.0f, 0.0f, width, height };

	m_PositionalLightPipeline1->GetRasterizerState().SetViewport(viewport);
	m_PositionalLightPipeline2->GetRasterizerState().SetViewport(viewport);
	s_GBufferPipeline->GetRasterizerState().SetViewport(viewport); 
	s_OpaquePipeline->GetRasterizerState().SetViewport(viewport);
	s_TransparentPipeline->GetRasterizerState().SetViewport(viewport);
	s_DebugPipeline->GetRasterizerState().SetViewport(viewport);
	s_SkyboxPipeline->GetRasterizerState().SetViewport(viewport);
	s_ParticlesPipelineState->GetRasterizerState().SetViewport(viewport);
	s_DeferredQuadPipeline->GetRasterizerState().SetViewport(viewport);

	s_DepthOnlyRenderTarget->Resize(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
	s_GBufferRenderTarget->Resize(static_cast<uint16_t>(width), static_cast<uint16_t>(height));

	s_ForwardTechnique->SetViewport(viewport);
	s_DeferredTechnique->SetViewport(viewport);
}

void DirectionalLightSystem(entt::registry& registry);
void PointLightsSystem(entt::registry& registry, const RenderParams& params);

void Renderer::Clear()
{
	Application::Get().GetWindow().GetContext()->GetRenderTarget()->Clear(ClearFlags::All);
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
			using namespace entt::literals;
			registry.ctx().emplace_as<Entity>("mainCameraEntity"_hs, Entity(entt::handle(registry, entity)));
			mainCamera = &camera.Camera;
			transform.RecalculateModel();
			cameraTransform = &transform;
			return;
		}
	}
}

RenderParams Renderer::CreateRenderParams(SceneCamera* mainCamera, Transform* cameraTransform)
{
	LEV_PROFILE_FUNCTION();

	cameraTransform->SetWorldScale(Vector3::One);
	const auto cameraViewMatrix = cameraTransform->GetModel().Invert();
	const auto cameraPosition = cameraTransform->GetWorldPosition();
	const auto perspectiveViewProjectionMatrix = cameraViewMatrix * mainCamera->GetPerspectiveProjection();
	return { mainCamera, cameraPosition, cameraViewMatrix, perspectiveViewProjectionMatrix };
}

void Renderer::Render(entt::registry& registry, SceneCamera* mainCamera, const Transform* cameraTransform)
{
	LEV_PROFILE_FUNCTION();

	if (!mainCamera)
	{
		Clear();
		return;
	}

	RecalculateAllTransforms(registry);

	mainCamera->RecalculateFrustum(*cameraTransform);
	
	const auto renderParams = CreateRenderParams(mainCamera, const_cast<Transform*>(cameraTransform));

	//TODO: Maybe move to its own pass?
	DirectionalLightSystem(registry);
	PointLightsSystem(registry, renderParams);
	Renderer3D::UpdateLights();

	//TODO: Maybe move to its own pass?
	Renderer3D::SetCameraBuffer(renderParams.Camera, renderParams.CameraViewMatrix, renderParams.CameraPosition);
	
	switch (RenderSettings::RenderTechnique)
	{
	case RenderTechniqueType::Forward:
		s_ForwardTechnique->Process(registry, renderParams);
		break;
	case RenderTechniqueType::Deferred:
		s_DeferredTechnique->Process(registry, renderParams);
		break;
	case RenderTechniqueType::ForwardPlus:
		LEV_NOT_IMPLEMENTED
		break;
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

void PointLightsSystem(entt::registry& registry, const RenderParams& params)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.group<>(entt::get<Transform, PointLightComponent>);
    for (const auto entity : view)
    {
        auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

		auto worldPosition = transform.GetWorldPosition();
		auto positionViewSpace = Vector4::Transform(Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f), params.CameraViewMatrix);
		Renderer3D::AddPointLights(positionViewSpace, worldPosition, light);
    }
}

}
