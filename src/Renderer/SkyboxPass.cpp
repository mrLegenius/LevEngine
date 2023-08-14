#include "pch.h"
#include "SkyboxPass.h"

#include "RenderCommand.h"
#include "Kernel/Application.h"
#include "Scene/Components/Components.h"

namespace LevEngine
{
struct alignas(16) CameraData
{
	Matrix View;
	Matrix ViewProjection;
	Vector3 Position;
};

SkyboxPass::SkyboxPass(const Ref<PipelineState>& pipeline) : m_SkyboxPipeline(pipeline)
{
	m_SkyboxMesh = CreateRef<SkyboxMesh>(ShaderAssets::Skybox());
	m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 0);
}

void SkyboxPass::Process(entt::registry& registry, RenderParams& params)
{
	const auto group = registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
	for (const auto entity : group)
	{
		auto [transform, skybox] = group.get<Transform, SkyboxRendererComponent>(entity);

		if (!skybox.texture) continue;

		m_SkyboxPipeline->Bind();
		
		skybox.texture->Bind(0, Shader::Type::Pixel);

		const CameraData skyboxCameraData{ Matrix::Identity, params.CameraPerspectiveViewProjectionMatrix, Vector3::Zero };
		m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
		m_CameraConstantBuffer->Bind(Shader::Type::Vertex);

		RenderCommand::DrawIndexed(m_SkyboxMesh->VertexBuffer, m_SkyboxMesh->IndexBuffer);

		skybox.texture->Unbind(0, Shader::Type::Pixel);
		m_SkyboxPipeline->Unbind();
		break;
	}
}
}
