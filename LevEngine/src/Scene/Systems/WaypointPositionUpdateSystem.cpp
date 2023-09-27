#include "levpch.h"
#include "WaypointPositionUpdateSystem.h"

#include "Scene/Components/Animation/WaypointMovement.h"
#include "Scene/Components/Transform/Transform.h"

void WaypointPositionUpdateSystem::Update(const float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<LevEngine::WaypointMovementComponent, LevEngine::Transform>();

    for (const auto entity : view)
    {
        auto [waypointMovement, transform] = view.get<LevEngine::WaypointMovementComponent, LevEngine::Transform>(entity);

        Vector<LevEngine::Entity> entities = waypointMovement.entities;
        Vector<Vector3> positions;
        double totalDistance = 0.0;
        
        for (auto waypointEntity : entities)
        {
            if (!waypointEntity.HasComponent<LevEngine::Transform>())
                continue;

            Vector3 position = waypointEntity.GetComponent<LevEngine::Transform>().GetWorldPosition();
            if (positions.size() > 0)
            {
                totalDistance += (position - positions.back()).Length();
            }
            positions.emplace_back(position);
        }

        const int positionsSize = positions.size();
        
        if (positionsSize == 0)
            return;

        for (int i = 1; i < positionsSize; ++i)
        {
            Vector3 prevPosition = positions[i - 1];
            Vector3 difference = (positions[i] - prevPosition);
            const float distance = difference.Length();
            if (totalDistance + LevEngine::Math::FloatEpsilon > distance)
            {
                totalDistance -= distance;
                continue;
            }

            Vector3 dir = difference;
            dir.Normalize();
            transform.SetWorldPosition(prevPosition + dir * distance);
        }
    }
}
