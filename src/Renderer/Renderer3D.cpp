#include "Renderer3D.h"
#include "RenderCommand.h"
#include "Assets.h"
#include "Debugging/Profiler.h"
#include "Kernel/Application.h"

Ref<D3D11ConstantBuffer> Renderer3D::m_ModelConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_CameraConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_LightningConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_ShadowMapConstantBuffer;
Ref<D3D11ConstantBuffer> Renderer3D::m_MaterialConstantBuffer;

Ref<D3D11ShadowMap> Renderer3D::m_ShadowMap;
Ref<D3D11CascadeShadowMap> Renderer3D::m_CascadeShadowMap;

Matrix Renderer3D::m_PerspectiveViewProjection;
Matrix Renderer3D::m_ViewProjection;
Ref<SkyboxMesh> Renderer3D::s_SkyboxMesh;

PointLightData Renderer3D::s_PointLights[RenderSettings::MaxPointLights];
LightningData Renderer3D::s_LightningData;
ShadowData Renderer3D::s_ShadowData;


struct alignas(16) CameraData
{
    Matrix View;
    Matrix ViewProjection;
    Vector3 Position;
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

	m_CameraConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof CameraData, 0);
	m_ModelConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof MeshModelBufferData, 1);
	m_LightningConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof LightningData, 2);
	m_ShadowMapConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof ShadowData, 3);
    m_MaterialConstantBuffer = CreateRef<D3D11ConstantBuffer>(sizeof MaterialData, 4);

    m_ShadowMap = CreateRef<D3D11ShadowMap>(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
    m_CascadeShadowMap = CreateRef<D3D11CascadeShadowMap>(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);

    s_SkyboxMesh = CreateRef<SkyboxMesh>(ShaderAssets::Skybox());

    s_LightningData.GlobalAmbient = RenderSettings::GlobalAmbient;
}

void Renderer3D::Shutdown()
{
    LEV_PROFILE_FUNCTION();
}

void Renderer3D::BeginShadowPass(SceneCamera& camera, const std::vector<Matrix> mainCameraProjections, const Matrix& mainCameraView)
{
    LEV_PROFILE_FUNCTION();

    for (int cascadeIndex = 0; cascadeIndex < RenderSettings::CascadeCount; ++cascadeIndex)
    {
        auto frustrumCorners = GetFrustumWorldCorners(mainCameraView, mainCameraProjections[cascadeIndex]);

        Vector4 center = Vector4::Zero;

        for (const auto& corner : frustrumCorners)
            center += corner;

        center /= frustrumCorners.size();

        const auto view = Matrix::CreateLookAt(static_cast<Vector3>(center), static_cast<Vector3>(center) + s_LightningData.DirLight.Direction, Vector3::Up);

        s_ShadowData.ViewProjection[cascadeIndex] = view * GetCascadeProjection(view, frustrumCorners);
        s_ShadowData.Distances[cascadeIndex] = camera.GetPerspectiveProjectionSliceDistance(RenderSettings::CascadeDistances[cascadeIndex]);

        s_ShadowData.ShadowMapDimensions = RenderSettings::ShadowMapResolution;
    }
    m_CascadeShadowMap->SetRenderTarget();
    ShaderAssets::CascadeShadowPass()->Bind();

    camera.SetViewportSize(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
    RenderCommand::SetViewport(0, 0, RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);

    m_ShadowMapConstantBuffer->SetData(&s_ShadowData, sizeof ShadowData);
    //s_ShadowPassData = { s_ShadowData.ViewProjection[cascadeIndex] };
    //m_ShadowPassConstantBuffer->SetData(&s_ShadowPassData, sizeof ShadowPassData);
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

    MaterialData material;
    material.Emissive = meshRenderer.material.Emissive;
    material.Ambient = meshRenderer.material.Ambient;
    material.Diffuse = meshRenderer.material.Diffuse;
    material.Specular = meshRenderer.material.Specular;
    material.Shininess = meshRenderer.material.Shininess;
    material.UseTexture = meshRenderer.material.UseTexture;

    m_MaterialConstantBuffer->SetData(&material);
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

    auto& data = s_LightningData.DirLight;
    data.Direction = dirLightDirection;
    data.Color = dirLight.Color;
}

void Renderer3D::AddPointLights(const Vector3& position, const PointLightComponent& pointLight)
{
    LEV_PROFILE_FUNCTION();

    if (s_LightningData.PointLightsCount >= RenderSettings::MaxPointLights)
    {
        std::cout << "Trying to add point light beyond maximum" << std::endl;
        return;
    }

    PointLightData& pointLightData = s_LightningData.PointLightsData[s_LightningData.PointLightsCount];

    pointLightData.Position = position;
    pointLightData.Color = pointLight.Color;
    pointLightData.Attenuation = Vector3{ pointLight.constant, pointLight.linear, pointLight.quadratic };

    s_LightningData.PointLightsCount++;
}

void Renderer3D::UpdateLights()
{
    LEV_PROFILE_FUNCTION();

    m_LightningConstantBuffer->SetData(&s_LightningData);
    s_LightningData.PointLightsCount = 0;
}
