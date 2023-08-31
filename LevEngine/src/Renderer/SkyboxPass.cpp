#include "levpch.h"
#include "SkyboxPass.h"

#include "RenderCommand.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"
#include "Scene/Components/Transform/Transform.h"

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
	m_SkyboxMesh = Mesh::CreateCube();
	m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 0);
}

void SkyboxPass::Process(entt::registry& registry, RenderParams& params)
{
	const auto group = registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
	for (const auto entity : group)
	{
		Transform transform = group.get<Transform>(entity);
		SkyboxRendererComponent skybox = group.get<SkyboxRendererComponent>(entity);

		if (!skybox.skybox) continue;

		const auto texture = skybox.skybox->GetTexture();

		if (!texture) continue;

		m_SkyboxPipeline->Bind();
		
		texture->Bind(0, Shader::Type::Pixel);

		const CameraData skyboxCameraData{ Matrix::Identity, params.CameraPerspectiveViewProjectionMatrix, Vector3::Zero };
		m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
		m_CameraConstantBuffer->Bind(Shader::Type::Vertex);

		m_SkyboxMesh->Bind(m_SkyboxPipeline->GetShader(Shader::Type::Vertex));

		RenderCommand::DrawIndexed(m_SkyboxMesh->IndexBuffer);

		texture->Unbind(0, Shader::Type::Pixel);
		m_SkyboxPipeline->Unbind();
		break;
	}
}
}
