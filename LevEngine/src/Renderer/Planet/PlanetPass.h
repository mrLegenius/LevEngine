#pragma once

#include "Kernel/Core.h"
#include "PlanetConstants.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
	class PipelineState;
	class Shader;
	class PlanetChunk;
	struct Transform;

	// Draws the ground.
	//
	// A pass of its own rather than a material on a mesh renderer, because a planet's chunks are not
	// entities: they come and go several times a second as the camera moves, and putting them in the
	// registry would mean creating and destroying hundreds of entities a frame and serializing them
	// into the scene file. So the chunks stay in the quadtree and this walks it.
	class LEV_API PlanetPass final : public RenderPass
	{
	public:
		// deferred picks which permutation of the surface shader is used, exactly as it does for
		// OpaquePass: the same geometry either fills the G-buffer or lights itself.
		PlanetPass(const Ref<PipelineState>& pipelineState, bool deferred);

	protected:
		String PassName() override;
		bool Begin(entt::registry& registry, RenderParams& params) override;
		void Process(entt::registry& registry, RenderParams& params) override;
		void End(entt::registry& registry, RenderParams& params) override;

	private:
		Ref<PipelineState> m_PipelineState;
		bool m_Deferred;

		PlanetConstants m_Constants;
	};
}
