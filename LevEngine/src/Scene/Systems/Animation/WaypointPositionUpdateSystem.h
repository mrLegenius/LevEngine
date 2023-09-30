#pragma once
#include "Scene/System.h"

class WaypointPositionUpdateSystem : LevEngine::System
{
public:
    void Update(float deltaTime, entt::registry& registry) override;
};
