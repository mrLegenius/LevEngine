#include "Renderer3D.h"
#include "RenderCommand.h"
#include "Assets.h"
#include "Debug/Profiler.h"
#include "Kernel/Application.h"

#define MAX_POINT_LIGHTS 10

Ref<D3D11ConstantBuffer> Renderer3D::m_ModelConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_DirLightConstantBuffer;

Matrix Renderer3D::m_PerspectiveViewProjection;
Matrix Renderer3D::m_ViewProjection;
Ref<SkyboxMesh> Renderer3D::s_SkyboxMesh;

std::vector<PointLightData> Renderer3D::s_PointLights;
DirLightData Renderer3D::s_DirLight;

struct alignas(16) CameraData
{
    Matrix ViewProjection;
    Vector3 Position;
};

struct alignas(16) LightningData
{
    DirLightData DirLight;
    PointLightData* PointLightsData;

    uint32_t PointLightsCount = 0;
};
void Renderer3D::Init()
{
    LEV_PROFILE_FUNCTION();

	m_CameraConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof CameraData);
	m_ModelConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof MeshModelBufferData, 1);
	m_DirLightConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof LightningData, 2);

    s_SkyboxMesh = CreateRef<SkyboxMesh>(ShaderAssets::Skybox());
}

void Renderer3D::Shutdown() { }
void Renderer3D::Shutdown()
{
    LEV_PROFILE_FUNCTION();
}

void Renderer3D::BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    RenderCommand::Begin();
    const auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

    m_ViewProjection = viewMatrix * camera.GetProjection();
    m_PerspectiveViewProjection = viewMatrix * camera.GetPerspectiveProjection();

    const CameraData cameraData{ m_ViewProjection, position };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
}

void Renderer3D::EndScene()
{
    LEV_PROFILE_FUNCTION();

    RenderCommand::End();
}

void Renderer3D::DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    if (!meshRenderer.mesh->VertexBuffer)
	    meshRenderer.mesh->Init(meshRenderer.shader->GetLayout());

    const MeshModelBufferData data = { model };
    m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));
    
    meshRenderer.texture->Bind();
    meshRenderer.shader->Bind();
    
    RenderCommand::DrawIndexed(meshRenderer.mesh->VertexBuffer, meshRenderer.mesh->IndexBuffer);
}

void Renderer3D::DrawSkybox(const SkyboxRendererComponent& renderer)
{
    LEV_PROFILE_FUNCTION();

    renderer.texture->Bind();
    ShaderAssets::Skybox()->Bind();

    const CameraData skyboxCameraData{ m_PerspectiveViewProjection, Vector3::Zero };
    m_CameraConstantBuffer->SetData(&skyboxCameraData, sizeof CameraData);

    RenderCommand::SetDepthFunc(DepthFunc::LessOrEqual);
    RenderCommand::DrawIndexed(s_SkyboxMesh->VertexBuffer, s_SkyboxMesh->IndexBuffer);
    RenderCommand::SetDepthFunc(DepthFunc::Less);
}

void Renderer3D::SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight)
{
    LEV_PROFILE_FUNCTION();

    s_DirLight.Direction = dirLightDirection;

    s_DirLight.Ambient = dirLight.Ambient;
    s_DirLight.Diffuse = dirLight.Diffuse;
    s_DirLight.Specular = dirLight.Specular;
}

void Renderer3D::AddPointLights(const Vector3& position, const PointLightComponent& pointLight)
{
    LEV_PROFILE_FUNCTION();

    if (s_PointLights.size() >= MAX_POINT_LIGHTS)
    {
        std::cout << "Trying to add point light beyond maximum" << std::endl;
        return;
    }

    PointLightData pointLightData{};

    pointLightData.Position = position;

    pointLightData.Constant = pointLight.constant;
    pointLightData.Linear = pointLight.linear;
    pointLightData.Quadratic = pointLight.quadratic;

    pointLightData.Ambient = pointLight.Ambient;
    pointLightData.Diffuse = pointLight.Diffuse;
    pointLightData.Specular = pointLight.Specular;

    s_PointLights.emplace_back(std::move(pointLightData));
}

void Renderer3D::UpdateLights()
{
    LEV_PROFILE_FUNCTION();

    const LightningData lightningData
    {
        s_DirLight,
        {s_PointLights.data()},
        s_PointLights.size(),
    };
    m_DirLightConstantBuffer->SetData(&lightningData, sizeof LightningData);
}
