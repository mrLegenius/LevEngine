#include "levpch.h"
#include "ShadowMapPass.h"

#include "Renderer/Lighting/CascadeShadowMap.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Shader/Shader.h"
#include "Assets/EngineAssets.h"
#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
Vector<Vector4> ShadowMapPass::GetFrustumWorldCorners(const Matrix& view, const Matrix& proj)
{
    const auto viewProj = view * proj;
    const auto inv = viewProj.Invert();

    Vector<Vector4> corners;
    corners.reserve(8);
    for (uint32_t x = 0; x < 2; ++x)
        for (uint32_t y = 0; y < 2; ++y)
            for (uint32_t z = 0; z < 2; ++z)
            {
                const Vector4 pt = Vector4::Transform(Vector4(
                    2.0f *  static_cast<float>(x) - 1.0f,
                    2.0f *  static_cast<float>(y) - 1.0f,
                    static_cast<float>(z),
                    1.0f), inv);

                corners.push_back(pt / pt.w);
            }

    return corners;
}

Matrix ShadowMapPass::GetCascadeProjection(const Matrix& lightView, Vector<Vector4> frustumCorners)
{
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();

    for (const auto& corner : frustumCorners)
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

ShadowMapPass::ShadowMapPass()
{
	LEV_PROFILE_FUNCTION();

	m_CascadeShadowMap = CreateRef<CascadeShadowMap>(
		static_cast<uint32_t>(RenderSettings::ShadowMapResolution),
		static_cast<uint32_t>(RenderSettings::ShadowMapResolution));
	m_ShadowMapConstantBuffer = ConstantBuffer::Create(sizeof ShadowData, 3);
}

String ShadowMapPass::PassName() { return "Shadow Map"; }

bool ShadowMapPass::Begin(entt::registry& registry, RenderParams& params)
{
	const auto group = registry.group<>(entt::get<Transform, DirectionalLightComponent>);

	if (group.empty()) return false;
	
    Vector3 lightDirection;
	for (const auto entity : group)
	{
		auto& transform = group.get<Transform>(entity);
        lightDirection = transform.GetForwardDirection();
	}

	const auto cameraCascadeProjections = params.Camera->GetSplitPerspectiveProjections(RenderSettings::CascadeDistances, RenderSettings::CascadeCount);

	for (int cascadeIndex = 0; cascadeIndex < RenderSettings::CascadeCount; ++cascadeIndex)
	{
		auto frustumCorners = GetFrustumWorldCorners(params.CameraViewMatrix, cameraCascadeProjections[cascadeIndex]);

		Vector4 center = Vector4::Zero;

		for (const auto& corner : frustumCorners)
			center += corner;

		center /= static_cast<float>(frustumCorners.size());

		const auto lightViewMatrix = Matrix::CreateLookAt(static_cast<Vector3>(center), static_cast<Vector3>(center) + lightDirection, Vector3::Up);

		m_ShadowData.ViewProjection[cascadeIndex] = lightViewMatrix * GetCascadeProjection(lightViewMatrix, frustumCorners);
		m_ShadowData.Distances[cascadeIndex] = params.Camera->GetPerspectiveProjectionSliceDistance(RenderSettings::CascadeDistances[cascadeIndex]);

		m_ShadowData.ShadowMapDimensions = RenderSettings::ShadowMapResolution;
	}
	m_CascadeShadowMap->SetRenderTarget();
	ShaderAssets::CascadeShadowPass()->Bind();
	
	m_ShadowMapConstantBuffer->SetData(&m_ShadowData, sizeof ShadowData);
    m_ShadowMapConstantBuffer->Bind(ShaderType::Geometry);

    return true;
}

void ShadowMapPass::Process(entt::registry& registry, RenderParams& params)
{
	// Process static meshes
	const auto staticMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent>, entt::exclude<AnimatorComponent>);
    for (const auto entity : staticMeshGroup)
    {
        auto [transform, mesh] = staticMeshGroup.get<Transform, MeshRendererComponent>(entity);

    	if (!mesh.enabled) continue;
		if (!mesh.mesh) continue;
		if (!mesh.material) continue;
    	
        if (mesh.castShadow)
            Renderer3D::DrawMesh(transform.GetModel(), mesh, ShaderAssets::CascadeShadowPass());
    }

	// Process animated meshes
	const auto animatedMeshGroup = registry.group<>(entt::get<Transform, MeshRendererComponent, AnimatorComponent>);
	for (const auto entity : animatedMeshGroup)
	{
		auto [transform, meshRenderer, animator] = animatedMeshGroup.get<Transform, MeshRendererComponent,
			AnimatorComponent>(entity);

		if (!meshRenderer.enabled) continue;
		if (!meshRenderer.mesh) continue;
		if (!meshRenderer.material) continue;
    	
		if (meshRenderer.castShadow)
		{
			Renderer3D::DrawMesh(transform.GetModel(), animator.GetFinalBoneMatrices(),  meshRenderer,
				ShaderAssets::CascadeShadowPass());	
		}
	}
}

void ShadowMapPass::End(entt::registry& registry, RenderParams& params)
{
    ShaderAssets::CascadeShadowPass()->Unbind();
    m_CascadeShadowMap->ResetRenderTarget();
    m_ShadowMapConstantBuffer->SetData(&m_ShadowData, sizeof ShadowData);
    m_ShadowMapConstantBuffer->Bind(ShaderType::Pixel);
    m_CascadeShadowMap->Bind(RenderSettings::ShadowMapSlot);
}
}