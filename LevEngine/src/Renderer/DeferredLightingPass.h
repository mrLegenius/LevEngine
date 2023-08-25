﻿#pragma once
#include "ConstantBuffer.h"
#include "PipelineState.h"
#include "RenderPass.h"
#include "Texture.h"

namespace LevEngine
{
	class DeferredLightingPass final : public RenderPass
	{
	public:
		DeferredLightingPass(
			const Ref<PipelineState>& pipeline1, 
			const Ref<PipelineState>& pipeline2,
			const Ref<Texture>& diffuseTexture,
			const Ref<Texture>& specularTexture,
			const Ref<Texture>& normalTexture,
			const Ref<Texture>& depthTexture)
				: m_Pipeline1(pipeline1)
				, m_Pipeline2(pipeline2)
				, m_DiffuseTexture(diffuseTexture)
				, m_SpecularTexture(specularTexture)
				, m_NormalTexture(normalTexture)
				, m_DepthTexture(depthTexture)
		{
			LEV_PROFILE_FUNCTION();

			m_LightIndexBuffer = ConstantBuffer::Create(sizeof m_LightParams, 4);
		}

		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void End(entt::registry& registry, RenderParams& params) override;
	private:
		struct alignas(16) LightParams
		{
			uint32_t LightIndex;
		};

		LightParams m_LightParams;

		Ref<PipelineState> m_Pipeline1;
		Ref<PipelineState> m_Pipeline2;

		Ref<Texture> m_DiffuseTexture;
		Ref<Texture> m_SpecularTexture;
		Ref<Texture> m_NormalTexture;
		Ref<Texture> m_DepthTexture;

		Ref<ConstantBuffer> m_LightIndexBuffer;
	};
}
