#pragma once

#include "RenderPass.h"

namespace LevEngine
{
	class ConstantBuffer;
	class Texture;
	class PipelineState;

	class DeferredLightingPass final : public RenderPass
	{
	public:
		DeferredLightingPass(
			const Ref<PipelineState>& pipeline1, 
			const Ref<PipelineState>& pipeline2,
			const Ref<Texture>& albedoMap,
			const Ref<Texture>& metallicRoughnessAOTexture,
			const Ref<Texture>& normalMap,
			const Ref<Texture>& depthMap);

		String PassName() override;
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void End(entt::registry& registry, RenderParams& params) override;
	private:
		struct alignas(16) LightParams
		{
			uint32_t LightIndex;
		};

		LightParams m_LightParams{};

		Ref<PipelineState> m_Pipeline1;
		Ref<PipelineState> m_Pipeline2;

		Ref<Texture> m_AlbedoMap;
		Ref<Texture> m_MetallicRoughnessAOMap;
		Ref<Texture> m_NormalMap;
		Ref<Texture> m_DepthMap;

		Ref<ConstantBuffer> m_LightIndexBuffer;
	};
}

