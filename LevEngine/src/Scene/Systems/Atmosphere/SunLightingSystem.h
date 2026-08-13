#pragma once

#include "Kernel/Core.h"
#include "Scene/System.h"

namespace LevEngine
{
    struct AtmosphereComponent;

    // Drives the directional light of every sun from the atmosphere it shines through, so the
    // scene lighting reddens and fades on its own as the sun sets.
    class LEV_API SunLightingSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;

        // The whole of the above, minus the frame. The editor calls this while a scene is only being
        // edited, where systems do not run but the sky is rendered all the same.
        static void Apply(entt::registry& registry);

        // Fraction of each colour channel that survives the trip from space down to the ground,
        // along the given direction towards the sun. Direction is expected to be normalized.
        [[nodiscard]] static Vector3 CalcSunTransmittance(const AtmosphereComponent& atmosphere,
                                                          const Vector3& directionToSun);
    };
}
