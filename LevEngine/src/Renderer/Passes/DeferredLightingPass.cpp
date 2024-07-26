#include "levpch.h"
#include "DeferredLightingPass.h"

#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Pipeline/RenderTarget.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Math/BoundingVolume.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/Transform/Transform.h"
#include "Renderer/Shader/ShaderType.h"

namespace LevEngine
{
	DeferredLightingPass::DeferredLightingPass(const Ref<PipelineState>& pipeline1, const Ref<PipelineState>& pipeline2,
		const Ref<Texture>& albedoMap, const Ref<Texture>& metallicRoughnessAOTexture, const Ref<Texture>& normalMap,
		const Ref<Texture>& depthMap): m_Pipeline1(pipeline1)
		                               , m_Pipeline2(pipeline2)
		                               , m_AlbedoMap(albedoMap)
		                               , m_MetallicRoughnessAOMap(metallicRoughnessAOTexture)
		                               , m_NormalMap(normalMap)
		                               , m_DepthMap(depthMap)
	{
		LEV_PROFILE_FUNCTION();

		m_LightIndexBuffer = ConstantBuffer::Create(sizeof m_LightParams, 4);
	}

	String DeferredLightingPass::PassName() { return "Lighting"; }

	bool DeferredLightingPass::Begin(entt::registry& registry, RenderParams& params)
	{
		m_AlbedoMap->Bind(1, ShaderType::Pixel);
		m_NormalMap->Bind(2, ShaderType::Pixel);
		m_MetallicRoughnessAOMap->Bind(3, ShaderType::Pixel);
		m_DepthMap->Bind(4, ShaderType::Pixel);

		return RenderPass::Begin(registry, params);
	}

	void DeferredLightingPass::Process(entt::registry& registry, RenderParams& params)
	{
		m_LightParams.LightIndex = 0;

		const auto view = registry.group<>(entt::get<Transform, PointLightComponent>);
		for (const auto entity : view)
		{
			auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

			auto worldPosition = transform.GetWorldPosition();
			
			if (RenderSettings::UseFrustumCulling)
			{
				auto sphereBoundingVolume = SphereBoundingVolume{worldPosition, light.Range};
				if (!sphereBoundingVolume.IsOnFrustum(params.Camera->GetFrustum()))
				{
					m_LightParams.LightIndex++;
					continue;
				}
			}
			
			auto model = Matrix::CreateScale(light.Range, light.Range, light.Range)
				* Matrix::CreateTranslation(worldPosition);

			m_Pipeline1->GetRenderTarget()->Clear( ClearFlags::Stencil, Vector4::Zero, 1.0f, 1 );
			
			m_LightIndexBuffer->SetData(&m_LightParams);
			m_LightIndexBuffer->Bind(ShaderType::Pixel);

			m_Pipeline1->Bind();
			Renderer3D::RenderSphere(model, m_Pipeline1->GetShader(ShaderType::Vertex));
			m_Pipeline1->Unbind();

			m_Pipeline2->Bind();
			Renderer3D::RenderSphere(model, m_Pipeline2->GetShader(ShaderType::Vertex));
			m_Pipeline2->Unbind();

			m_LightParams.LightIndex++;
		}

		const auto spotLightView = registry.view<Transform, SpotLightComponent>();
		for (const auto entity : spotLightView)
		{
			auto [transform, light] = spotLightView.get<Transform, SpotLightComponent>(entity);

			auto worldPosition = transform.GetWorldPosition();

			//TODO: Make bounding box instead of sphere
			if (RenderSettings::UseFrustumCulling)
			{
				auto sphereBoundingVolume = SphereBoundingVolume{worldPosition, light.Range};
				if (!sphereBoundingVolume.IsOnFrustum(params.Camera->GetFrustum()))
				{
					m_LightParams.LightIndex++;
					continue;
				}
			}

			const float angle = tanf(light.Angle * Math::DegToRad);
			const float scaleY = angle * light.Range;
			const float scaleX = angle * light.Range;
			const float scaleZ = light.Range;

			auto model =
				  Matrix::CreateScale(scaleX, scaleY, scaleZ)
				* Matrix::CreateFromQuaternion(transform.GetWorldRotation())
				* Matrix::CreateTranslation(worldPosition);

			m_Pipeline1->GetRenderTarget()->Clear(ClearFlags::Stencil, Vector4::Zero, 1.0f, 1);
			
			m_LightIndexBuffer->SetData(&m_LightParams);
			m_LightIndexBuffer->Bind(ShaderType::Pixel);

			m_Pipeline1->Bind();
			Renderer3D::RenderCone(model, m_Pipeline1->GetShader(ShaderType::Vertex));
			m_Pipeline1->Unbind();

			m_Pipeline2->Bind();
			Renderer3D::RenderCone(model, m_Pipeline2->GetShader(ShaderType::Vertex));
			m_Pipeline2->Unbind();

			m_LightParams.LightIndex++;
		}
	}

	void DeferredLightingPass::End(entt::registry& registry, RenderParams& params)
	{
		m_AlbedoMap->Unbind(1, ShaderType::Pixel);
		m_NormalMap->Unbind(2, ShaderType::Pixel);
		m_MetallicRoughnessAOMap->Unbind(3, ShaderType::Pixel);
		m_DepthMap->Unbind(4, ShaderType::Pixel);
	}
}

