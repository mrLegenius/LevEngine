#include "levpch.h"
#include "CelestialOrbitSystem.h"

#include "Scene/Components/Atmosphere/Atmosphere.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void CelestialOrbitSystem::Update(const float deltaTime, entt::registry& registry)
    {
        LEV_PROFILE_FUNCTION();

        const auto view = registry.view<Transform, CelestialOrbitComponent>();
        for (const auto entity : view)
        {
            auto [transform, orbit] = view.get<Transform, CelestialOrbitComponent>(entity);

            if (orbit.AutoAdvance && orbit.DayLength > 0.0f)
            {
                orbit.TimeOfDay += deltaTime / orbit.DayLength;
            }

            // Keep it in 0:1 no matter how it was edited -- the inspector and scripts both write
            // this field directly, and a drifting value would slowly lose float precision. Whatever
            // is wrapped away is kept as a day, since the star field counts its drift in those.
            const float completedDays = std::floor(orbit.TimeOfDay);
            orbit.DaysElapsed += completedDays;
            orbit.TimeOfDay -= completedDays;

            ApplyOrbit(transform, orbit);
        }
    }

    void CelestialOrbitSystem::ApplyOrbit(Transform& transform, const CelestialOrbitComponent& orbit)
    {
        // The transform's forward is the direction the light travels, which is away from the body.
        const Vector3 lightDirection = -orbit.GetDirectionToBody();
        transform.SetWorldRotation(Quaternion::FromToRotation(Vector3::Forward, lightDirection));
    }
}
