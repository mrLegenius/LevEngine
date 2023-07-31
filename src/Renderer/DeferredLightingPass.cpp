#include "pch.h"
#include "DeferredLightingPass.h"

#include "Renderer3D.h"
#include "Scene/Components/Components.h"

namespace LevEngine
{
	bool DeferredLightingPass::Begin(entt::registry& registry, RenderParams& params)
	{
		m_DiffuseTexture->Bind(1, Shader::Type::Pixel);
		m_SpecularTexture->Bind(2, Shader::Type::Pixel);
		m_NormalTexture->Bind(3, Shader::Type::Pixel);
		m_DepthTexture->Bind(4, Shader::Type::Pixel);

		return RenderPass::Begin(registry, params);
	}

	void DeferredLightingPass::Process(entt::registry& registry, RenderParams& params)
	{
		const auto view = registry.group<>(entt::get<Transform, PointLightComponent>);
		for (const auto entity : view)
		{
			auto [transform, light] = view.get<Transform, PointLightComponent>(entity);

			auto worldPosition = transform.GetWorldPosition();
			auto model = Matrix::CreateScale(light.Range, light.Range, light.Range)
				* Matrix::CreateTranslation(worldPosition);

			auto positionViewSpace = Vector4::Transform(Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f), params.CameraViewMatrix);
			Renderer3D::AddPointLights(positionViewSpace, worldPosition, light);

			m_Pipeline1->Bind();
			Renderer3D::RenderSphere(model);
			m_Pipeline1->Unbind();

			m_Pipeline2->Bind();
			Renderer3D::ResetLights();
			Renderer3D::RenderSphere(model);
			m_Pipeline2->Unbind();
		}
	}

	void DeferredLightingPass::End(entt::registry& registry, RenderParams& params)
	{
		m_DiffuseTexture->Unbind(1, Shader::Type::Pixel);
		m_SpecularTexture->Unbind(2, Shader::Type::Pixel);
		m_NormalTexture->Unbind(3, Shader::Type::Pixel);
		m_DepthTexture->Unbind(4, Shader::Type::Pixel);
	}
}

