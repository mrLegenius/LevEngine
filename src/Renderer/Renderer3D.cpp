#include "pch.h"
#include "Renderer3D.h"
#include "RenderCommand.h"
#include "Assets.h"
#include "Debugging/Profiler.h"
#include "Kernel/Application.h"
namespace LevEngine
{
Ref<ConstantBuffer> Renderer3D::m_ModelConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_LightningConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_MaterialConstantBuffer;
Ref<ConstantBuffer> Renderer3D::m_ScreenToViewParamsConstantBuffer;

Ref<D3D11DeferredTechnique> Renderer3D::m_GBuffer;
Ref<D3D11ForwardTechnique> Renderer3D::s_ForwardTechnique;

Matrix Renderer3D::m_ViewProjection;
Ref<SkyboxMesh> Renderer3D::s_SkyboxMesh;

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
    m_MaterialConstantBuffer = ConstantBuffer::Create(sizeof MaterialData, 4);
    m_ScreenToViewParamsConstantBuffer = ConstantBuffer::Create(sizeof ScreenToViewParams, 5);

    s_SkyboxMesh = CreateRef<SkyboxMesh>(ShaderAssets::Skybox());

    s_LightningData.GlobalAmbient = RenderSettings::GlobalAmbient;

    const auto& window = Application::Get().GetWindow();
    m_GBuffer = CreateRef<D3D11DeferredTechnique>(window.GetWidth(), window.GetHeight());
    s_ForwardTechnique = CreateRef<D3D11ForwardTechnique>();
}

void Renderer3D::Shutdown()
{
    LEV_PROFILE_FUNCTION();
}

void Renderer3D::BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    s_ForwardTechnique->StartOpaquePass();
    const auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

    m_ViewProjection = viewMatrix * camera.GetProjection();

    const CameraData cameraData{  viewMatrix, m_ViewProjection, position };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
}

void Renderer3D::EndScene()
{
    LEV_PROFILE_FUNCTION();

    s_ForwardTechnique->End();
}

inline void TryUnbindTexture(const Ref<Texture> texture, const int slot)
{
    if (texture)
    {
        texture->Unbind(slot);
    }
}

inline void TryBindTexture(const Ref<Texture> texture, int& hasTexture, const int slot)
{
    if (texture)
    {
        hasTexture = true;
        texture->Bind(slot);
    }
}

void Renderer3D::DrawOpaqueMesh(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    MaterialData material;
    material.Emissive = meshRenderer.material.Emissive;
    material.Ambient = meshRenderer.material.Ambient;
    material.Diffuse = meshRenderer.material.Diffuse;
    material.Specular = meshRenderer.material.Specular;
    material.Shininess = meshRenderer.material.Shininess;

    TryBindTexture(meshRenderer.emissiveTexture, material.HasEmissiveTexture, 0);
    TryBindTexture(meshRenderer.ambientTexture, material.HasAmbientTexture, 1);
    TryBindTexture(meshRenderer.diffuseTexture, material.HasDiffuseTexture, 2);
    TryBindTexture(meshRenderer.specularTexture, material.HasSpecularTexture, 3);
    TryBindTexture(meshRenderer.normalTexture, material.HasNormalTexture, 4);

    m_MaterialConstantBuffer->SetData(&material);

    meshRenderer.shader->Bind();
    DrawMesh(model, meshRenderer);
}

void Renderer3D::DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    if (!meshRenderer.mesh->VertexBuffer)
        meshRenderer.mesh->Init(meshRenderer.shader->GetLayout());

    const MeshModelBufferData data = { model };
    m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));

    RenderCommand::DrawIndexed(meshRenderer.mesh->VertexBuffer, meshRenderer.mesh->IndexBuffer);
}

void Renderer3D::DrawSkybox(const SkyboxRendererComponent& renderer, const Matrix& perspectiveViewProjection)
{
    LEV_PROFILE_FUNCTION();

    const CameraData skyboxCameraData{ Matrix::Identity, perspectiveViewProjection, Vector3::Zero };
    m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);

    RenderCommand::DrawIndexed(s_SkyboxMesh->VertexBuffer, s_SkyboxMesh->IndexBuffer);
}

void Renderer3D::DrawDeferredMesh(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    if (!meshRenderer.mesh->VertexBuffer)
        meshRenderer.mesh->Init(ShaderAssets::GBufferPass()->GetLayout());

    MaterialData material;
    material.Emissive = meshRenderer.material.Emissive;
    material.Ambient = meshRenderer.material.Ambient;
    material.Diffuse = meshRenderer.material.Diffuse;
    material.Specular = meshRenderer.material.Specular;
    material.Shininess = meshRenderer.material.Shininess;

    TryBindTexture(meshRenderer.emissiveTexture, material.HasEmissiveTexture, 0);
    TryBindTexture(meshRenderer.ambientTexture, material.HasAmbientTexture, 1);
    TryBindTexture(meshRenderer.diffuseTexture, material.HasDiffuseTexture, 2);
    TryBindTexture(meshRenderer.specularTexture, material.HasSpecularTexture, 3);
    TryBindTexture(meshRenderer.normalTexture, material.HasNormalTexture, 4);

    m_MaterialConstantBuffer->SetData(&material);

    DrawMesh(model, meshRenderer);

    TryUnbindTexture(meshRenderer.emissiveTexture,0);
    TryUnbindTexture(meshRenderer.ambientTexture, 1);
    TryUnbindTexture(meshRenderer.diffuseTexture, 2);
    TryUnbindTexture(meshRenderer.specularTexture,3);
    TryUnbindTexture(meshRenderer.normalTexture, 4);
}

void Renderer3D::BeginDeferred(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    m_GBuffer->StartOpaquePass();

    const auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

    m_ViewProjection = viewMatrix * camera.GetProjection();

    const CameraData cameraData{ viewMatrix, m_ViewProjection, position };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
    UpdateLights();
}

void Renderer3D::BeginDeferredDirLightningSubPass(const SceneCamera& camera)
{
    LEV_PROFILE_FUNCTION();

    m_GBuffer->StartDirectionalLightingPass();

    const auto& window = Application::Get().GetWindow();
    const float width = window.GetWidth();
    const float height = window.GetHeight();
    const ScreenToViewParams params{ camera.GetProjection().Invert(), Vector2{width, height}};
    m_ScreenToViewParamsConstantBuffer->SetData(&params);
    UpdateLights();
}

void Renderer3D::BeginDeferredPositionalLightningSubPass(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    const auto& window = Application::Get().GetWindow();
    const float width = window.GetWidth();
    const float height = window.GetHeight();
    const ScreenToViewParams params{ camera.GetProjection().Invert(), Vector2{width, height} };
    m_ScreenToViewParamsConstantBuffer->SetData(&params);

    m_ViewProjection = viewMatrix * camera.GetProjection();

    const CameraData cameraData{ viewMatrix, m_ViewProjection, position };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
}

void Renderer3D::BeginDeferredPositionalLightningSubPass1(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    m_GBuffer->StartPositionalLightingPass1();
}

void Renderer3D::BeginDeferredPositionalLightningSubPass2(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    m_GBuffer->StartPositionalLightingPass2();
    s_LightningData.PointLightsCount = 0;
    UpdateLights();
}


void Renderer3D::EndDeferredLightningPass()
{
    m_GBuffer->EndLightningPass();
}

void Renderer3D::EndDeferred()
{
    
}

void Renderer3D::SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight)
{
    LEV_PROFILE_FUNCTION();

    auto& data = s_LightningData.DirLight;
    data.Direction = dirLightDirection;
    data.Color = dirLight.Color;
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

    RenderCommand::DrawIndexed(mesh->VertexBuffer, mesh->IndexBuffer);
}
}
