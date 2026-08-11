#pragma once

#include "Kernel/Core.h"
#include "Scene/System.h"

namespace LevEngine
{
    class LEV_API AIAgentCrowdUpdateSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;

    private:
        float m_updatePeriod = 0.5f;
        float m_deltaTimeCounter = 0.0f;
    };
}
