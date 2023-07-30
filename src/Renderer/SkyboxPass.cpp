#include "pch.h"
#include "SkyboxPass.h"

#include "RenderCommand.h"
#include "Kernel/Application.h"
namespace LevEngine
{
struct alignas(16) CameraData
{
	Matrix View;
	Matrix ViewProjection;
	Vector3 Position;
};

SkyboxPass::SkyboxPass() 
{
	//Pipeline
	{
		//TODO: maybe pass by parameter
		const auto mainRenderTarget = Application::Get().GetWindow().GetContext()->GetRenderTarget();

		m_SkyboxPipeline.SetRenderTarget(mainRenderTarget);

		const DepthMode depthMode{ true, DepthWrite::Enable, CompareFunction::LessOrEqual };
		m_SkyboxPipeline.GetRasterizerState().SetCullMode(CullMode::None);
		m_SkyboxPipeline.GetRasterizerState().SetDepthClipEnabled(false);
		m_SkyboxPipeline.GetDepthStencilState()->SetDepthMode(depthMode);
		m_SkyboxPipeline.SetShader(Shader::Type::Vertex, ShaderAssets::Skybox());
		m_SkyboxPipeline.SetShader(Shader::Type::Pixel, ShaderAssets::Skybox());
	}

	m_SkyboxMesh = CreateRef<SkyboxMesh>(ShaderAssets::Skybox());
	m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 0);
}

void SkyboxPass::Process(entt::registry& registry, RenderParams& params)
{
	m_SkyboxPipeline.Bind();

	const auto group = registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
	for (const auto entity : group)
	{
		auto [transform, skybox] = group.get<Transform, SkyboxRendererComponent>(entity);
		skybox.texture->Bind(0, Shader::Type::Pixel);

		const CameraData skyboxCameraData{ Matrix::Identity, params.CameraPerspectiveViewProjectionMatrix, Vector3::Zero };
		m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
		m_CameraConstantBuffer->Bind(Shader::Type::Vertex);

		RenderCommand::DrawIndexed(m_SkyboxMesh->VertexBuffer, m_SkyboxMesh->IndexBuffer);

		skybox.texture->Unbind(0, Shader::Type::Pixel);

		m_SkyboxPipeline.Unbind();
		break;
	}
}
}
