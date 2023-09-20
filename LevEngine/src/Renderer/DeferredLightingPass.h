#pragma once
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
			const Ref<Texture>& albedoMap,
			const Ref<Texture>& metallicRoughnessAOTexture,
			const Ref<Texture>& normalMap,
			const Ref<Texture>& depthMap)
				: m_Pipeline1(pipeline1)
				, m_Pipeline2(pipeline2)
				, m_AlbedoMap(albedoMap)
				, m_MetallicRoughnessAOMap(metallicRoughnessAOTexture)
				, m_NormalMap(normalMap)
				, m_DepthMap(depthMap)
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

		Ref<Texture> m_AlbedoMap;
		Ref<Texture> m_MetallicRoughnessAOMap;
		Ref<Texture> m_NormalMap;
		Ref<Texture> m_DepthMap;

		Ref<ConstantBuffer> m_LightIndexBuffer;
	};
}

