#include "levpch.h"
#include "WaypointPositionUpdateSystem.h"

#include "Scene/Components/Animation/WaypointMovement.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void WaypointPositionUpdateSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<WaypointMovementComponent, Transform>();

        for (const auto entity : view)
        {
            auto [waypointMovement, transform] = view.get<WaypointMovementComponent, Transform>(entity);

            Vector<Entity> entities = waypointMovement.entities;
            Vector<Transform*> waypoints;
            double totalDistance = 0.0;
        
            for (auto waypointEntity : entities)
            {
                if (!waypointEntity.HasComponent<Transform>())
                    continue;

                Transform& waypointTransform = waypointEntity.GetComponent<Transform>();
                Vector3 position = waypointTransform.GetWorldPosition();
                if (waypoints.size() > 0)
                {
                    totalDistance += (position - waypoints.back()->GetWorldPosition()).Length();
                }
                waypoints.emplace_back(&waypointTransform);
            }

            const int waypointsCount = waypoints.size();
        
            if (waypointsCount == 0)
                return;

            double traveledDistance = totalDistance * waypointMovement.currentDisplacement;

            for (int i = 1; i < waypointsCount; ++i)
            {
                Vector3 prevPosition = waypoints[i - 1]->GetWorldPosition();
                Vector3 difference = (waypoints[i]->GetWorldPosition() - prevPosition);
                const float distance = difference.Length();
                if (traveledDistance + Math::FloatEpsilon > distance)
                {
                    traveledDistance -= distance;
                    continue;
                }

                Vector3 dir = difference;
                dir.Normalize();
                
                transform.SetWorldPosition(prevPosition + dir * traveledDistance);
                
                transform.SetWorldRotation(Quaternion::Slerp(waypoints[i - 1]->GetWorldRotation(),
                    waypoints[i]->GetWorldRotation(), traveledDistance / distance));

                break;
            }
        }
    }
}
