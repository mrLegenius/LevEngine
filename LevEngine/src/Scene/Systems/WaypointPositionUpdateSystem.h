#pragma once
#include "Scene/System.h"

class WaypointPositionUpdateSystem : LevEngine::System
{
public:
    void Update(const float deltaTime, entt::registry& registry) override;
};
