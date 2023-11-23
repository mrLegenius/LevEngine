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
#include "DataTypes/Array.h"

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
	m_SkyboxMesh = Primitives::CreateCube();
	m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 6);
}

void SkyboxPass::Process(entt::registry& registry, RenderParams& params)
{
	const auto group = registry.group<>(entt::get<Transform, SkyboxRendererComponent>);
	for (const auto entity : group)
	{
		Transform transform = group.get<Transform>(entity);
		SkyboxRendererComponent skybox = group.get<SkyboxRendererComponent>(entity);

		if (!skybox.SkyboxTexture) return;
		
		const auto& texture = skybox.SkyboxTexture->GetTexture();

		if (!texture) continue;

		CreateCubeMapSubPass(texture);
		
		const CameraData skyboxCameraData{ Matrix::Identity, params.CameraPerspectiveViewProjectionMatrix, Vector3::Zero };
		m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);
		
		m_SkyboxPipeline->GetShader(ShaderType::Pixel)->GetShaderParameterByName("tex").Set(m_CubemapTexture);
		m_SkyboxPipeline->GetShader(ShaderType::Vertex)->GetShaderParameterByName("CameraConstantBuffer").Set(m_CameraConstantBuffer);
		m_CameraConstantBuffer->Bind(6, ShaderType::Vertex);
		m_SkyboxPipeline->Bind();
		
		m_SkyboxMesh->Bind(m_SkyboxPipeline->GetShader(ShaderType::Vertex));
		RenderCommand::DrawIndexed(m_SkyboxMesh->IndexBuffer);

		m_SkyboxPipeline->Unbind();
		break;
	}
}

void SkyboxPass::CreateCubeMapSubPass(const Ref<Texture>& skyboxTexture)
{
	//Create CubeMap
	const Texture::TextureFormat format {Texture::Components::RGBA, Texture::Type::Float, 1, 16, 16, 16, 16};
	m_CubemapTexture = Texture::CreateTextureCube(512, 512, format);
	const auto renderTarget = RenderTarget::Create();
	renderTarget->AttachTexture(AttachmentPoint::Color0, m_CubemapTexture);
	const auto pipe = CreateRef<PipelineState>();
	pipe->SetShader(ShaderType::Pixel, ShaderAssets::EquirectangularToCubemap());
	pipe->SetShader(ShaderType::Vertex, ShaderAssets::EquirectangularToCubemap());
	pipe->SetShader(ShaderType::Geometry, ShaderAssets::EquirectangularToCubemap());
	pipe->SetRenderTarget(renderTarget);
	pipe->GetRasterizerState().SetCullMode(CullMode::None);
	pipe->GetRasterizerState().SetDepthClipEnabled(false);

	const Matrix captureProjection = Matrix::CreatePerspectiveFieldOfView(Math::PiDiv2, 1.0f, 0.1f, 10.0f);

	Array<Matrix, 6> captureViews = 
	{
		Matrix::CreateLookAt(Vector3::Zero, Vector3::Right, Vector3::Down) * captureProjection,
		Matrix::CreateLookAt(Vector3::Zero, Vector3::Left, Vector3::Down) * captureProjection,
		Matrix::CreateLookAt(Vector3::Zero, Vector3::Down, Vector3::Forward) * captureProjection,
		Matrix::CreateLookAt(Vector3::Zero, Vector3::Up, Vector3::Backward) * captureProjection,
		Matrix::CreateLookAt(Vector3::Zero, Vector3::Backward, Vector3::Down) * captureProjection,
		Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Down) * captureProjection
	};

	const auto constantBuffer = ConstantBuffer::Create(sizeof(Matrix) * 6, 6);
	constantBuffer->SetData(captureViews.data());
	
	pipe->GetShader(ShaderType::Pixel)->GetShaderParameterByName("equirectangularMap").Set(skyboxTexture);
	pipe->GetShader(ShaderType::Geometry)->GetShaderParameterByName("cubeMapConstantBuffer").Set(constantBuffer);

	pipe->GetRasterizerState().SetViewport({ 0, 0, 512, 512 });
	pipe->Bind();

	m_SkyboxMesh->Bind(m_SkyboxPipeline->GetShader(ShaderType::Vertex));
	RenderCommand::DrawIndexed(m_SkyboxMesh->IndexBuffer);
	
	pipe->Unbind();
}
}
