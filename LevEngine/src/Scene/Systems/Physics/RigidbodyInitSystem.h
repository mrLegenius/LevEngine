#pragma once
#include "Scene/System.h"

namespace LevEngine
{
    class RigidbodyInitSystem : public System
    {
    public:
        // TODO: Replace that is called once on play start when InitSystems will be implemented.
        void Update(float deltaTime, entt::registry& registry) override;
    };
}