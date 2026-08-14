#pragma once

#include "Kernel/Core.h"
#include "PlanetConstants.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
	class PipelineState;
	class Shader;

	// Draws the sea, from the terrain's own chunk meshes pulled out to sea level. See PlanetOcean.hlsl
	// for why it has no geometry of its own.
	//
	// Runs with the transparent pipeline, after everything opaque, because water is see-through and
	// what is under it has to be there already. Only chunks with ground below sea level are submitted;
	// the rest would be clipped away pixel by pixel for nothing.
	class LEV_API PlanetOceanPass final : public RenderPass
	{
	public:
		explicit PlanetOceanPass(const Ref<PipelineState>& pipelineState);

	protected:
		String PassName() override;
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void End(entt::registry& registry, RenderParams& params) override;

	private:
		Ref<PipelineState> m_PipelineState;

		PlanetConstants m_Constants;
	};
}
