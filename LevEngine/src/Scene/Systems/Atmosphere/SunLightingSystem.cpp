#include "levpch.h"
#include "SunLightingSystem.h"

#include "Scene/Components/Atmosphere/Atmosphere.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    // Steps used to integrate the optical depth from the ground to the edge of the atmosphere.
    // This runs once per sun per frame on the CPU, so it can afford to be a plain march.
    static constexpr int k_TransmittanceSteps = 24;

    namespace
    {
        // Distance to where a ray leaves a sphere centred on the origin, or -1 when it misses.
        float RayExitDistance(const Vector3& origin, const Vector3& direction, const float radius)
        {
            const float b = origin.Dot(direction);
            const float c = origin.Dot(origin) - radius * radius;
            const float discriminant = b * b - c;

            if (discriminant < 0.0f) return -1.0f;

            return -b + std::sqrt(discriminant);
        }
    }

    Vector3 SunLightingSystem::CalcSunTransmittance(const AtmosphereComponent& atmosphere,
                                                    const Vector3& directionToSun)
    {
        const Vector3 origin{0.0f, atmosphere.PlanetRadius, 0.0f};
        const float atmosphereRadius = atmosphere.GetAtmosphereRadius();

        const float exitDistance = RayExitDistance(origin, directionToSun, atmosphereRadius);
        if (exitDistance <= 0.0f) return Vector3::Zero;

        const float stepSize = exitDistance / static_cast<float>(k_TransmittanceSteps);

        float rayleighDepth = 0.0f;
        float mieDepth = 0.0f;
        float absorptionDepth = 0.0f;

        for (int i = 0; i < k_TransmittanceSteps; ++i)
        {
            const Vector3 samplePosition = origin + directionToSun * ((static_cast<float>(i) + 0.5f) * stepSize);
            const float height = samplePosition.Length() - atmosphere.PlanetRadius;

            rayleighDepth += std::exp(-height / atmosphere.RayleighScaleHeight) * stepSize;
            mieDepth += std::exp(-height / atmosphere.MieScaleHeight) * stepSize;

            // Tent shaped layer, the same profile the shader uses for ozone.
            const float absorption = Math::Max(
                0.0f, 1.0f - std::abs(height - atmosphere.AbsorptionLayerCenter) / atmosphere.AbsorptionLayerWidth);
            absorptionDepth += absorption * stepSize;
        }

        const float mieExtinction = atmosphere.MieScattering + atmosphere.MieAbsorption;

        Vector3 opticalDepth = atmosphere.RayleighScattering * rayleighDepth
            + Vector3{mieExtinction, mieExtinction, mieExtinction} * mieDepth
            + atmosphere.AbsorptionCoefficients * absorptionDepth;

        return {std::exp(-opticalDepth.x), std::exp(-opticalDepth.y), std::exp(-opticalDepth.z)};
    }

    void SunLightingSystem::Update(float deltaTime, entt::registry& registry)
    {
        Apply(registry);
    }

    void SunLightingSystem::Apply(entt::registry& registry)
    {
        LEV_PROFILE_FUNCTION();

        const AtmosphereComponent* atmosphere = nullptr;
        const auto atmosphereView = registry.view<AtmosphereComponent>();
        for (const auto entity : atmosphereView)
        {
            atmosphere = &atmosphereView.get<AtmosphereComponent>(entity);
            break;
        }

        const auto suns = registry.view<Transform, SunComponent, DirectionalLightComponent>();
        for (const auto entity : suns)
        {
            auto [transform, sun, light] = suns.get<Transform, SunComponent, DirectionalLightComponent>(entity);

            if (!sun.DrivesDirectionalLight) continue;

            const Vector3 directionToSun = -transform.GetForwardDirection();

            Vector3 transmittance = Vector3::One;
            if (atmosphere)
            {
                transmittance = CalcSunTransmittance(*atmosphere, directionToSun);

                // The sun is a disk, not a point, so it takes its own angular radius to go from
                // fully up to fully set. Without this the scene lighting would switch off in one
                // frame at the exact moment the centre crosses the horizon.
                const float angularRadius = std::sin(sun.AngularDiameter * 0.5f * Math::DegToRad);
                const float horizonFade = Math::Clamp(
                    (directionToSun.y + angularRadius) / (2.0f * angularRadius + Math::FloatEpsilon), 0.0f, 1.0f);

                transmittance *= horizonFade;
            }

            const Vector3 baseColor = static_cast<Vector3>(sun.Color) * sun.LightIntensity;
            const Vector3 result = baseColor * transmittance;

            light.color = Color(result.x, result.y, result.z, 1.0f);
        }
    }
}
