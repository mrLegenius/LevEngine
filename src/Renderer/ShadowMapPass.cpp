#include "pch.h"
#include "ShadowMapPass.h"
#include "RenderCommand.h"
namespace LevEngine
{
std::vector<Vector4> ShadowMapPass::GetFrustumWorldCorners(const Matrix& view, const Matrix& proj) const
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

Matrix ShadowMapPass::GetCascadeProjection(const Matrix& lightView, std::vector<Vector4> frustrumCorners) const
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

        minX = Math::Min(minX, trf.x);
        maxX = Math::Max(maxX, trf.x);
        minY = Math::Min(minY, trf.y);
        maxY = Math::Max(maxY, trf.y);
        minZ = Math::Min(minZ, trf.z);
        maxZ = Math::Max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;

    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

    return Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
}

bool ShadowMapPass::Begin(entt::registry& registry, RenderParams& params)
{
	const SceneCamera* lightCamera = nullptr;
	const auto group = registry.group<>(entt::get<Transform, DirectionalLightComponent, CameraComponent>);
    Vector3 lightDirection;
	for (const auto entity : group)
	{
		auto [transform, camera] = group.get<Transform, CameraComponent>(entity);
		lightCamera = &camera.camera;
        lightDirection = transform.GetForwardDirection();
	}

	if (!lightCamera) return false;

	const auto cameraCascadeProjections = params.Camera.GetSplitPerspectiveProjections(RenderSettings::CascadeDistances, RenderSettings::CascadeCount);

	for (int cascadeIndex = 0; cascadeIndex < RenderSettings::CascadeCount; ++cascadeIndex)
	{
		auto frustrumCorners = GetFrustumWorldCorners(params.CameraViewMatrix, cameraCascadeProjections[cascadeIndex]);

		Vector4 center = Vector4::Zero;

		for (const auto& corner : frustrumCorners)
			center += corner;

		center /= frustrumCorners.size();

		const auto lightViewMatrix = Matrix::CreateLookAt(static_cast<Vector3>(center), static_cast<Vector3>(center) + lightDirection, Vector3::Up);

		m_ShadowData.ViewProjection[cascadeIndex] = lightViewMatrix * GetCascadeProjection(lightViewMatrix, frustrumCorners);
		m_ShadowData.Distances[cascadeIndex] = params.Camera.GetPerspectiveProjectionSliceDistance(RenderSettings::CascadeDistances[cascadeIndex]);

		m_ShadowData.ShadowMapDimensions = RenderSettings::ShadowMapResolution;
	}
	m_CascadeShadowMap->SetRenderTarget();
	ShaderAssets::CascadeShadowPass()->Bind();

	params.Camera.SetViewportSize(RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);
	RenderCommand::SetViewport(0, 0, RenderSettings::ShadowMapResolution, RenderSettings::ShadowMapResolution);

	m_ShadowMapConstantBuffer->SetData(&m_ShadowData, sizeof ShadowData);
    m_ShadowMapConstantBuffer->Bind(Shader::Type::Geometry);

    return true;
}

void ShadowMapPass::Process(entt::registry& registry, RenderParams& params)
{
	const auto view = registry.group<>(entt::get<Transform, MeshRendererComponent>);
    for (const auto entity : view)
    {
        auto [transform, mesh] = view.get<Transform, MeshRendererComponent>(entity);
        if (mesh.castShadow)
            Renderer3D::DrawMesh(transform.GetModel(), mesh);
    }
}

void ShadowMapPass::End(entt::registry& registry, RenderParams& params)
{
    ShaderAssets::CascadeShadowPass()->Unbind();
    m_CascadeShadowMap->ResetRenderTarget();
    m_ShadowMapConstantBuffer->SetData(&m_ShadowData, sizeof ShadowData);
    m_ShadowMapConstantBuffer->Bind(Shader::Type::Pixel);
    m_CascadeShadowMap->Bind(RenderSettings::ShadowMapSlot);
}
}
