#include "levpch.h"
#include "PlanetUpdatePass.h"

#include "Renderer/RenderParams.h"
#include "Scene/Components/Planet/Planet.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	String PlanetUpdatePass::PassName() { return "Planet Update"; }

	void PlanetUpdatePass::Process(entt::registry& registry, RenderParams& params)
	{
		LEV_PROFILE_FUNCTION();

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

			planet.Surface->Update(observer);
		}
	}
}
