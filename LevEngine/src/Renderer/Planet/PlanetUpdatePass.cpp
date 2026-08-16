#include "levpch.h"
#include "PlanetUpdatePass.h"

#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/RenderParams.h"
#include "Scene/Components/Planet/Planet.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	String PlanetUpdatePass::PassName() { return "Planet Update"; }

	void PlanetUpdatePass::Process(entt::registry& registry, RenderParams& params)
	{
		LEV_PROFILE_FUNCTION();

		// Pixels per radian of the vertical field of view: the viewport's height over the angle it
		// covers. A wider lens or a smaller window puts fewer pixels on every triangle, and the level
		// of detail should follow both rather than being tuned for one window.
		const float fieldOfView = Math::Max(params.Camera->GetFieldOfView(), 1.0f) * Math::DegToRad;
		const float viewportHeight = Math::Max(m_Viewport.height, 1.0f);
		const float pixelsPerRadian = viewportHeight / fieldOfView;

		const auto planets = registry.group<>(entt::get<Transform, PlanetComponent>);

		for (const auto entity : planets)
		{
			auto [transform, planet] = planets.get<Transform, PlanetComponent>(entity);

			if (!planet.Surface) continue;

			//<--- Settings may have been edited since the last frame; only what changed is rebuilt ---<<
			planet.ApplySettings();

			// The quadtree works in the planet's own space, so the camera has to be brought into it --
			// otherwise a planet that has been moved or turned subdivides in the wrong place.
			const Matrix planetToWorld = transform.GetModel();
			const Matrix worldToPlanet = planetToWorld.Invert();

			const Vector3 observer = Vector3::Transform(params.CameraPosition, worldToPlanet);

			// Renderer::Render rebuilds the frustum from the camera's transform just before the
			// techniques run, so this is the frustum for the frame about to be drawn rather than the
			// last one. It is in world space, hence the matrix and the scale alongside it.
			PlanetCullingView culling;
			culling.ViewFrustum = &params.Camera->GetFrustum();
			culling.PlanetToWorld = planetToWorld;
			culling.WorldScale = Math::MaxElement(transform.GetWorldScale());

			planet.Surface->Update(observer, pixelsPerRadian, culling);
		}
	}
}
