#pragma once

#include "Kernel/Core.h"
#include "Renderer/Passes/RenderPass.h"

namespace LevEngine
{
	// Walks each planet's quadtree against the camera and leaves behind the list of chunks to draw.
	//
	// A render pass rather than a scene system, because the level of detail has to follow the camera
	// the frame is actually being rendered from -- which in the editor is the viewport camera and not
	// any camera in the scene. A system would only ever see the scene's own, and the ground would
	// tessellate itself around wherever the player is standing rather than where you are looking.
	//
	// First in the technique, so the shadow pass and the surface pass work from the same list.
	class LEV_API PlanetUpdatePass final : public RenderPass
	{
	public:
		PlanetUpdatePass() = default;

		//<--- The level of detail is measured in pixels, and only this knows how many there are ---<<
		void SetViewport(Viewport viewport) override { m_Viewport = viewport; }

	protected:
		String PassName() override;
		void Process(entt::registry& registry, RenderParams& params) override;

	private:
		Viewport m_Viewport{};
	};
}
