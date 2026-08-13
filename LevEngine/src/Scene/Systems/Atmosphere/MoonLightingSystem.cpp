#include "levpch.h"
#include "MoonLightingSystem.h"

#include "SunLightingSystem.h"
#include "Scene/Components/Atmosphere/Atmosphere.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void MoonLightingSystem::Update(float deltaTime, entt::registry& registry)
    {
        Apply(registry);
    }

    void MoonLightingSystem::Apply(entt::registry& registry)
    {
        LEV_PROFILE_FUNCTION();

        const AtmosphereComponent* atmosphere = nullptr;
        const auto atmosphereView = registry.view<AtmosphereComponent>();
        for (const auto entity : atmosphereView)
        {
            atmosphere = &atmosphereView.get<AtmosphereComponent>(entity);
            break;
        }

        const auto moons = registry.view<Transform, MoonComponent, DirectionalLightComponent>();
        for (const auto entity : moons)
        {
            auto [transform, moon, light] = moons.get<Transform, MoonComponent, DirectionalLightComponent>(entity);

            if (!moon.DrivesDirectionalLight) continue;

            const Vector3 directionToMoon = -transform.GetForwardDirection();
            const MoonLighting::Illumination illumination = MoonLighting::Collect(registry, moon, directionToMoon);

            Vector3 result = illumination.Radiance * illumination.PhaseFactor * moon.LightIntensity;

            if (atmosphere)
            {
                // Moonlight goes through the same air the sunlight does, so it reddens and dies at
                // the horizon in the same way, over the moon's own angular size.
                result *= SunLightingSystem::CalcSunTransmittance(*atmosphere, directionToMoon);

                const float angularRadius = std::sin(moon.AngularDiameter * 0.5f * Math::DegToRad);
                const float horizonFade = Math::Clamp(
                    (directionToMoon.y + angularRadius) / (2.0f * angularRadius + Math::FloatEpsilon), 0.0f, 1.0f);

                result *= horizonFade;
            }

            light.color = Color(result.x, result.y, result.z, 1.0f);
        }
    }
}
