#pragma once
#include "Physics/Components/Rigidbody.h"
#include "Scene/System.h"

namespace LevEngine
{
    class RigidbodyInitSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;
    };
}
