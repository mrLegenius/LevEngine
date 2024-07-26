#pragma once
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
	class RenderTarget;
	class ConstantBuffer;
	class Texture;
	class PipelineState;

	class EnvironmentRenderPass final : public RenderPass
	{
	public:
		explicit EnvironmentRenderPass(const Ref<RenderTarget>& renderTarget);

		void SetEnvironmentMap(const Ref<Texture>& environmentMap);

		String PassName() override;
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void SetViewport(Viewport viewport) override;

	private:
        Ref<PipelineState> m_SkyboxPipeline;
        Ref<Texture> m_EnvironmentMap;
        Ref<ConstantBuffer> m_CameraConstantBuffer;
    };
}
