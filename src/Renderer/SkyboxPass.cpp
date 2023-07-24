#include "SkyboxPass.h"
#include "Kernel/Application.h"

SkyboxPass::SkyboxPass(entt::registry& registry) : m_Registry(registry)
{
	//Pipeline
	{
		//TODO: maybe pass by parameter
		auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();

		m_SkyboxPipeline.SetRenderTarget(mainRenderTarget);

		DepthMode depthMode{ true, DepthWrite::Enable, CompareFunction::LessOrEqual };
		m_SkyboxPipeline.GetRasterizerState().SetCullMode(CullMode::None);
		m_SkyboxPipeline.GetRasterizerState().SetDepthClipEnabled(false);
		m_SkyboxPipeline.GetDepthStencilState().SetDepthMode(depthMode);
	}
}

void SkyboxPass::Process(RenderParams& params)
{
	m_SkyboxPipeline.Bind();
	ShaderAssets::Skybox()->Bind();

	const auto group = m_Registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
	for (const auto entity : group)
	{
		auto [transform, skybox] = group.get<Transform, SkyboxRendererComponent>(entity);
		skybox.texture->Bind();
		Renderer3D::DrawSkybox(skybox, params.CameraPerspectiveViewProjectionMatrix);
		skybox.texture->Unbind();

		ShaderAssets::Skybox()->Unbind();
		m_SkyboxPipeline.Unbind();
		break;
	}
}
