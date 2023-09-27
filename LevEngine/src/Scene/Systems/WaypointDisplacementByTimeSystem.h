#pragma once
#include "Scene/System.h"

namespace LevEngine
{
    class WaypointDisplacementByTimeSystem : public System
    {
    public:
        void Update(const float deltaTime, entt::registry& registry) override;
        
    private:
        bool m_Inited;
    };
}

