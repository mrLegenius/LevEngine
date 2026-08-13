#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
	class AtmosphereConstants;
	class RenderTarget;
	class ConstantBuffer;
	class Texture;
	class PipelineState;

	class LEV_API EnvironmentRenderPass final : public RenderPass
	{
	public:
		EnvironmentRenderPass(const Ref<RenderTarget>& renderTarget, const Ref<AtmosphereConstants>& atmosphere);

		void SetEnvironmentMap(const Ref<Texture>& environmentMap);

		String PassName() override;
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void SetViewport(Viewport viewport) override;

	private:
		[[nodiscard]] bool IsAtmosphereActive() const;

        Ref<PipelineState> m_SkyboxPipeline;
        Ref<PipelineState> m_AtmospherePipeline;
        Ref<AtmosphereConstants> m_Atmosphere;
        Ref<Texture> m_EnvironmentMap;
        Ref<ConstantBuffer> m_CameraConstantBuffer;
    };
}
