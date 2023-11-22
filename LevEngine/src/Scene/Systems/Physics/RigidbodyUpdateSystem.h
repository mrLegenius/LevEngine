#pragma once
#include "Math/Vector3.h"
#include "Scene/System.h"

namespace LevEngine
{
    class RigidbodyUpdateSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;

    private:
        Vector3 m_TransformScale = Vector3::One;
    };
}