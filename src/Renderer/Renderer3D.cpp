#include "pch.h"
#include "Renderer3D.h"
#include "RenderCommand.h"
#include "Assets.h"
#include "Kernel/Application.h"

namespace LevEngine
{
Ref<ConstantBuffer> Renderer3D::m_ModelConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_LightningConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_ScreenToViewParamsConstantBuffer;

Matrix Renderer3D::m_ViewProjection;

PointLightData Renderer3D::s_PointLights[RenderSettings::MaxPointLights];
LightningData Renderer3D::s_LightningData;

struct alignas(16) CameraData
{
    Matrix View;
    Matrix ViewProjection;
    Vector3 Position;
};

struct alignas(16) ScreenToViewParams
{
    Matrix CameraInvertedProjection;
    Vector2 ScreenDimensions;
};

void Renderer3D::Init()
{
    LEV_PROFILE_FUNCTION();

	m_CameraConstantBuffer = ConstantBuffer::Create(sizeof CameraData, 0);
	m_ModelConstantBuffer = ConstantBuffer::Create(sizeof MeshModelBufferData, 1);
	m_LightningConstantBuffer = ConstantBuffer::Create(sizeof LightningData, 2);
    m_ScreenToViewParamsConstantBuffer = ConstantBuffer::Create(sizeof ScreenToViewParams, 5);

    s_LightningData.GlobalAmbient = RenderSettings::GlobalAmbient;
}

void Renderer3D::SetCameraBuffer(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    const auto& window = Application::Get().GetWindow();
    const float width = window.GetWidth();
    const float height = window.GetHeight();

    RenderCommand::SetViewport(0, 0, width, height);

    const auto viewProjection = viewMatrix * camera.GetProjection();

    const CameraData cameraData{  viewMatrix, viewProjection, position };
    m_CameraConstantBuffer->SetData(&cameraData);
    m_CameraConstantBuffer->Bind(Shader::Type::Vertex | Shader::Type::Pixel);

    const ScreenToViewParams params{ camera.GetProjection().Invert(), Vector2{width, height} };
    m_ScreenToViewParamsConstantBuffer->SetData(&params);
    m_ScreenToViewParamsConstantBuffer->Bind(Shader::Type::Pixel);
}

void Renderer3D::DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    if (!meshRenderer.mesh->VertexBuffer)
        meshRenderer.mesh->Init(meshRenderer.shader->GetLayout());

    const MeshModelBufferData data = { model };
    m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
    m_ModelConstantBuffer->Bind(Shader::Type::Vertex);

    RenderCommand::DrawIndexed(meshRenderer.mesh->VertexBuffer, meshRenderer.mesh->IndexBuffer);
}

void Renderer3D::SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight)
{
    LEV_PROFILE_FUNCTION();

    s_LightningData.DirLight.Direction = dirLightDirection;
    s_LightningData.DirLight.Color = dirLight.Color;
}

void Renderer3D::AddPointLights(const Vector4& positionViewSpace, const Vector3& position, const PointLightComponent& pointLight)
{
    LEV_PROFILE_FUNCTION();

    if (s_LightningData.PointLightsCount >= RenderSettings::MaxPointLights)
    {
        Log::CoreWarning("Trying to add point light beyond maximum");
        return;
    }

    PointLightData& pointLightData = s_LightningData.PointLightsData[s_LightningData.PointLightsCount];

    pointLightData.PositionViewSpace = positionViewSpace;
    pointLightData.Position = position;
    pointLightData.Color = pointLight.Color;
    pointLightData.Range = pointLight.Range;
    pointLightData.Intensity = pointLight.Intensity;
    pointLightData.Smoothness = pointLight.Smoothness;

    s_LightningData.PointLightsCount++;
}

void Renderer3D::UpdateLights()
{
    LEV_PROFILE_FUNCTION();

    m_LightningConstantBuffer->SetData(&s_LightningData);
    m_LightningConstantBuffer->Bind(Shader::Type::Pixel);
    s_LightningData.PointLightsCount = 0;
}

void Renderer3D::RenderSphere(const Matrix& model)
{
    LEV_PROFILE_FUNCTION();

    static Ref<Mesh> mesh;
    if (mesh == nullptr)
    {
        mesh = Mesh::CreateSphere(10);
        mesh->Init(ShaderAssets::DeferredVertexOnly()->GetLayout());
    }

    const MeshModelBufferData data = { model };
    m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
    m_ModelConstantBuffer->Bind(Shader::Type::Vertex);

    RenderCommand::DrawIndexed(mesh->VertexBuffer, mesh->IndexBuffer);
}
}
