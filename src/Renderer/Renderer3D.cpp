#include "Renderer3D.h"
#include "RenderCommand.h"
#include "Assets.h"
#include "Debugging/Profiler.h"
#include "Kernel/Application.h"

Ref<D3D11ConstantBuffer> Renderer3D::m_ModelConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_DirLightConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_ShadowMapConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_ShadowPassConstantBuffer;

Ref<D3D11ShadowMap> Renderer3D::m_ShadowMap;
Ref<D3D11CascadeShadowMap> Renderer3D::m_CascadeShadowMap;

Matrix Renderer3D::m_PerspectiveViewProjection;
Matrix Renderer3D::m_ViewProjection;
Ref<SkyboxMesh> Renderer3D::s_SkyboxMesh;

std::vector<PointLightData> Renderer3D::s_PointLights;
DirLightData Renderer3D::s_DirLight;
ShadowData Renderer3D::s_ShadowData;
ShadowPassData Renderer3D::s_ShadowPassData;


struct alignas(16) CameraData
{
    Matrix View;
    Matrix ViewProjection;
    Vector3 Position;
};

struct alignas(16) LightningData
{
    DirLightData DirLight;
    PointLightData* PointLightsData;

    uint32_t PointLightsCount = 0;
};

std::vector<Vector4> GetFrustumWorldCorners(const Matrix& view, const Matrix& proj)
{
    const auto viewProj = view * proj;
    const auto inv = viewProj.Invert();

    std::vector<Vector4> corners;
    corners.reserve(8);
    for (uint32_t x = 0; x < 2; ++x)
        for (uint32_t y = 0; y < 2; ++y)
            for (uint32_t z = 0; z < 2; ++z)
            {
                const Vector4 pt = Vector4::Transform(Vector4(
                    2.0f * x - 1.0f,
                    2.0f * y - 1.0f,
                    z,
                    1.0f), inv);

                corners.push_back(pt / pt.w);
            }

    return corners;
}

Matrix GetCascadeProjection(const Matrix& lightView, std::vector<Vector4> frustrumCorners)
{
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    for (const auto& corner : frustrumCorners)
    {
        const auto trf = Vector4::Transform(corner, lightView);

        minX = LevEngine::Math::Min(minX, trf.x);
        maxX = LevEngine::Math::Max(maxX, trf.x);
        minY = LevEngine::Math::Min(minY, trf.y);
        maxY = LevEngine::Math::Max(maxY, trf.y);
        minZ = LevEngine::Math::Min(minZ, trf.z);
        maxZ = LevEngine::Math::Max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;

    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

    return Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
}

void Renderer3D::Init()
{
    LEV_PROFILE_FUNCTION();

	m_CameraConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof CameraData);
	m_ModelConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof MeshModelBufferData, 1);
	m_DirLightConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof LightningData, 2);
	m_ShadowMapConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof ShadowData, 3);
    m_ShadowPassConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof ShadowPassData, 3);

    m_ShadowMap = CreateRef<D3D11ShadowMap>(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
    m_CascadeShadowMap = CreateRef<D3D11CascadeShadowMap>(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);

    s_SkyboxMesh = CreateRef<SkyboxMesh>(ShaderAssets::Skybox());
}

void Renderer3D::Shutdown()
{
    LEV_PROFILE_FUNCTION();
}

void Renderer3D::BeginShadowPass(SceneCamera& camera, const Matrix& mainCameraProjection, const Matrix& mainCameraView, int cascadeIndex)
{
    LEV_PROFILE_FUNCTION();

    auto frustrumCorners = GetFrustumWorldCorners(mainCameraView, mainCameraProjection);

    Vector4 center = Vector4::Zero;

    for (const auto& corner : frustrumCorners)
        center += corner;

    center /= frustrumCorners.size();

    const auto view = Matrix::CreateLookAt(static_cast<Vector3>(center), static_cast<Vector3>(center) + s_DirLight.Direction, Vector3::Up);

    s_ShadowData.ViewProjection[cascadeIndex] = view * GetCascadeProjection(view, frustrumCorners);
    s_ShadowData.distances[cascadeIndex] = camera.GetPerspectiveProjectionSliceDistance(RenderSettings::CascadeDistances[cascadeIndex]);

    s_ShadowData.shadowMapDimensions = RenderSettings::ShadowMapResolution;

    m_CascadeShadowMap->SetRenderTarget(cascadeIndex);
    ShaderAssets::ShadowPass()->Bind();

    camera.SetViewportSize(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
    RenderCommand::SetViewport(0, 0, RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);

    //m_ShadowMapConstantBuffer->SetData(&s_ShadowData, sizeof ShadowData);
    s_ShadowPassData = { s_ShadowData.ViewProjection[cascadeIndex] };
    m_ShadowPassConstantBuffer->SetData(&s_ShadowPassData, sizeof ShadowPassData);
}

void Renderer3D::EndShadowPass()
{
    LEV_PROFILE_FUNCTION();

    RenderCommand::End();
}

void Renderer3D::BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position)
{
    LEV_PROFILE_FUNCTION();

    RenderCommand::Begin();
    const auto& window = Application::Get().GetWindow();
    RenderCommand::SetViewport(0, 0, window.GetWidth(), window.GetHeight());

    m_ViewProjection = viewMatrix * camera.GetProjection();
    m_PerspectiveViewProjection = viewMatrix * camera.GetPerspectiveProjection();

    const CameraData cameraData{  viewMatrix, m_ViewProjection, position };
    m_CameraConstantBuffer->SetData(&cameraData, sizeof CameraData);
    m_ShadowMapConstantBuffer->SetData(&s_ShadowData, sizeof ShadowData);
}

void Renderer3D::EndScene()
{
    LEV_PROFILE_FUNCTION();

    RenderCommand::End();
}

void Renderer3D::DrawMeshShadow(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    if (!meshRenderer.mesh->VertexBuffer)
        meshRenderer.mesh->Init(meshRenderer.shader->GetLayout());

    const MeshModelBufferData data = { model };
    m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));

    RenderCommand::DrawIndexed(meshRenderer.mesh->VertexBuffer, meshRenderer.mesh->IndexBuffer);
}

void Renderer3D::DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer)
{
    LEV_PROFILE_FUNCTION();

    if (!meshRenderer.mesh->VertexBuffer)
	    meshRenderer.mesh->Init(meshRenderer.shader->GetLayout());

    const MeshModelBufferData data = { model };
    m_ModelConstantBuffer->SetData(&data, sizeof(MeshModelBufferData));

    m_CascadeShadowMap->Bind(1);
    meshRenderer.texture->Bind();
    meshRenderer.shader->Bind();
    
    RenderCommand::DrawIndexed(meshRenderer.mesh->VertexBuffer, meshRenderer.mesh->IndexBuffer);
}

void Renderer3D::DrawSkybox(const SkyboxRendererComponent& renderer)
{
    LEV_PROFILE_FUNCTION();

    renderer.texture->Bind();
    ShaderAssets::Skybox()->Bind();

    const CameraData skyboxCameraData{  Matrix::Identity, m_PerspectiveViewProjection, Vector3::Zero };
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

    if (s_PointLights.size() >= RenderSettings::MaxPointLights)
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
