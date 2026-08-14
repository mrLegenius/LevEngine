#pragma once

#include "Kernel/Core.h"
#include "Scene/System.h"

namespace LevEngine
{
	// Gives the ground under everything that could stand on it a collider.
	//
	// Unlike the level of detail, which follows the camera and so lives in a render pass, collision
	// follows the simulation: what needs ground under it is a body that could fall through, and where
	// the camera happens to be looking has nothing to do with it. A planet seen from orbit with a
	// character walking on the far side needs the collider on the far side.
	class LEV_API PlanetCollisionSystem final : public System
	{
	public:
		void Update(float deltaTime, entt::registry& registry) override;
	};
}
