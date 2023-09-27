#include "levpch.h"
#include "WaypointDisplacementByTimeSystem.h"

#include "Scene/Components/Animation/WaypointMovement.h"
#include "Scene/Components/Time/TimelineComponent.h"
#include "Scene/Components/Transform/Transform.h"

void LevEngine::WaypointDisplacementByTimeSystem::Update(const float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<WaypointMovementComponent, TimelineComponent>();

    for (const auto entity : view)
    {
        auto [waypointMovement, timelineComponent] = view.get<WaypointMovementComponent, TimelineComponent>(entity);

        if (!m_Inited)
        {
            timelineComponent.Init();
            m_Inited = true;
        }

        waypointMovement.currentDisplacement = timelineComponent.GetElapsedTime() / timelineComponent.duration;
    }
}
