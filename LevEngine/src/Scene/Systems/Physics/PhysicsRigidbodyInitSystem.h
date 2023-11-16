#pragma once
#include "Scene/System.h"

namespace LevEngine
{
    class PhysicsRigidbodyInitSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;
    };
}
