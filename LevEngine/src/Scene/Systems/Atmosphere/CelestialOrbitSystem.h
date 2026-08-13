#pragma once

#include "Kernel/Core.h"
#include "Scene/System.h"

namespace LevEngine
{
    struct CelestialOrbitComponent;
    struct Transform;

    // Advances the day cycle and points every orbiting body's transform along its light direction.
    class LEV_API CelestialOrbitSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;

        // Points a transform along the orbit. The editor calls this too, so that scrubbing the
        // time of day moves the sun without having to enter play mode.
        static void ApplyOrbit(Transform& transform, const CelestialOrbitComponent& orbit);
    };
}
