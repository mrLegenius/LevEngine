#include "levpch.h"
#include "SkyboxPass.h"

#include "PipelineState.h"
#include "RenderCommand.h"
#include "3D/Mesh.h"
#include "3D/Primitives.h"
#include "Assets/SkyboxAsset.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"
#include "Scene/Components/Transform/Transform.h"
#include "Assets/TextureAsset.h"

#include "ConstantBuffer.h"
#include "EnvironmentPass.h"
#include "PipelineState.h"

#include "3D/Primitives.h"
#include "3D/Mesh.h"

namespace LevEngine
{
struct alignas(16) CameraData
{
	Matrix View;
	Matrix ViewProjection;
	Vector3 Position;
};

SkyboxPass::SkyboxPass(const Ref<PipelineState>& pipeline, const Ref<EnvironmentPass>& environmentPass)
	: m_SkyboxPipeline(pipeline)
	, m_EnvironmentPass(environmentPass)
	, m_SkyboxMesh(Primitives::CreateCube())
	, m_CameraConstantBuffer( ConstantBuffer::Create(sizeof CameraData, 6))
{
	
}

bool SkyboxPass::Begin(entt::registry& registry, RenderParams& params)
{
	return m_EnvironmentPass->GetEnvironmentCubemap() != nullptr;
}

void SkyboxPass::Process(entt::registry& registry, RenderParams& params)
{
		const auto& texture = m_EnvironmentPass->GetEnvironmentCubemap();

		const CameraData skyboxCameraData{ Matrix::Identity, params.CameraPerspectiveViewProjectionMatrix, Vector3::Zero };
		m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
		
		m_SkyboxPipeline->GetShader(ShaderType::Pixel)->GetShaderParameterByName("Cubemap").Set(texture);
		m_SkyboxPipeline->GetShader(ShaderType::Vertex)->GetShaderParameterByName("CameraConstantBuffer").Set(m_CameraConstantBuffer);
		m_CameraConstantBuffer->Bind(6, ShaderType::Vertex);
		m_SkyboxPipeline->Bind();
		
		m_SkyboxMesh->Bind(m_SkyboxPipeline->GetShader(ShaderType::Vertex));
		RenderCommand::DrawIndexed(m_SkyboxMesh->IndexBuffer);

		m_SkyboxPipeline->Unbind();
}
}
