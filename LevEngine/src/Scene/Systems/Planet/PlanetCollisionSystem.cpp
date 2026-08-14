#include "levpch.h"
#include "PlanetCollisionSystem.h"

#include "Planet/PlanetCollision.h"
#include "Planet/PlanetSurface.h"
#include "Physics/Components/CharacterController.h"
#include "Physics/Components/Rigidbody.h"
#include "Scene/Components/Planet/Planet.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	void PlanetCollisionSystem::Update(float deltaTime, entt::registry& registry)
	{
		LEV_PROFILE_FUNCTION();

		const auto planets = registry.group<>(entt::get<Transform, PlanetComponent>);
		if (planets.begin() == planets.end()) return;

		// Everything that could fall. A static rigidbody is not one of them -- it is not going
		// anywhere, and a wall standing on a planet needs no ground under it to stay where it is.
		Vector<Vector3> focusPoints;

		const auto bodies = registry.view<Transform, Rigidbody>();
		for (const auto entity : bodies)
		{
			auto [transform, rigidbody] = bodies.get<Transform, Rigidbody>(entity);

			if (rigidbody.GetRigidbodyType() == Rigidbody::Type::Static) continue;

			focusPoints.emplace_back(transform.GetWorldPosition());
		}

		const auto controllers = registry.view<Transform, CharacterController>();
		for (const auto entity : controllers)
		{
			auto [transform, controller] = controllers.get<Transform, CharacterController>(entity);

			focusPoints.emplace_back(transform.GetWorldPosition());
		}

		for (const auto entity : planets)
		{
			auto [transform, planet] = planets.get<Transform, PlanetComponent>(entity);

			if (!planet.GenerateCollision)
			{
				//<--- Switched off since the last frame: give the actors back rather than leak them ---<<
				if (planet.Collision)
					planet.Collision.reset();

				continue;
			}

			if (!planet.Surface) continue;

			if (!planet.Collision)
				planet.Collision = CreateRef<PlanetCollision>();

			planet.Collision->Update(*planet.Surface, transform, focusPoints, planet.CollisionRadius);
		}
	}
}
