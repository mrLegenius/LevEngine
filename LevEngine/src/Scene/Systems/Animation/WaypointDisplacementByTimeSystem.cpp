#include "levpch.h"
#include "WaypointDisplacementByTimeSystem.h"

#include "Scene/Components/Animation/WaypointMovement.h"
#include "Scene/Components/Time/TimelineComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void WaypointDisplacementByTimeSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<WaypointMovementComponent, TimelineComponent>();

        for (const auto entity : view)
        {
            auto [waypointMovement, timelineComponent] = view.get<WaypointMovementComponent, TimelineComponent>(entity);

            if (!timelineComponent.IsInitialized())
            {
                timelineComponent.Init();
            }

            waypointMovement.currentDisplacement = static_cast<float>
                (Math::Clamp(timelineComponent.GetElapsedTime() / timelineComponent.GetDuration(), 0.0, 1.0));
        }
    }
}
