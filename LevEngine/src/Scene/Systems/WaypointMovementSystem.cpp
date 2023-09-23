#include "levpch.h"
#include "WaypointMovementSystem.h"

#include "Scene/Components/Animation/WaypointMovement.h"
#include "Scene/Components/Transform/Transform.h"

void LevEngine::WaypointMovementSystem::Update(const float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<Transform, WaypointMovementComponent>();

    for (const auto entity : view)
    {
        auto [transform, waypointMovement] = view.get<Transform, WaypointMovementComponent>(entity);

        
    }
}
