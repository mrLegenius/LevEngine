#pragma once
#include "Scene/System.h"

namespace LevEngine
{
    class WaypointMovementSystem : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override;
    };
}

