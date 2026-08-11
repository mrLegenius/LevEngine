#pragma once

#include "Kernel/Core.h"
#include "Scene/System.h"

namespace LevEngine
{
    class LEV_API AnimatorUpdateSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;
    };
}
