#include "levpch.h"
#include "AtmosphereConstants.h"

#include "Kernel/Time/Time.h"
#include "Renderer/RenderParams.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Shader/ShaderType.h"
#include "Scene/Components/Atmosphere/Atmosphere.h"
#include "Scene/Components/Planet/Planet.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    // Slot the sky shaders read the planet from. See CB_ATMOSPHERE in Registers.hlsli.
    static constexpr uint32_t k_AtmosphereConstantBufferSlot = 10;

    // How far the camera has to climb before the light probe is worth rebuilding for the new
    // height, in kilometres. Fifty metres is far below the point where the sky visibly changes.
    // How far the camera may climb or fall before the baked sky is worth rendering again, as a
    // fraction of whichever is larger: its own altitude, or the thickness of the atmosphere.
    //
    // Relative rather than absolute, and that matters more than it looks. This used to be a flat
    // 0.05 km, which worked while the value it guarded was a height in metres converted to kilometres.
    // It is now a position in the atmosphere's own space, which on a game sized planet is scaled by
    // tens of kilometres per world unit -- so an absolute threshold in kilometres tripped on a
    // thousandth of a unit of movement, and the sky was re-rendered every frame the camera moved.
    // Which is why moving cost thirty times what standing still did.
    //
    // Relative also gets the two regimes right on its own: far out in space the sky hardly changes
    // with position, so the tolerance is huge and it almost never rebuilds; down near the ground the
    // tolerance is a fraction of the air's thickness, which is where altitude actually shows.
    //
    // Loose, and it can afford to be. At 0.05 this was a compromise between two symptoms of one
    // mistake: tighten it and the sky was rebuilt every frame the camera moved, loosen it and the
    // baked halo visibly lagged the planet. Neither is a thing any more -- the halo is marched per
    // pixel and what the snapshot guards is a light probe, where a quarter of the shell's thickness
    // of altitude buys nothing the diffuse ambient can show.
    static constexpr float k_ViewHeightTolerance = 0.25f;

    AtmosphereConstants::AtmosphereConstants()
        : m_ConstantBuffer(ConstantBuffer::Create(sizeof GPUAtmosphereData, k_AtmosphereConstantBufferSlot))
    {
    }

    bool AtmosphereConstants::Collect(entt::registry& registry, const RenderParams& params)
    {
        LEV_PROFILE_FUNCTION();

        m_Active = false;

        const AtmosphereComponent* atmosphere = nullptr;
        entt::entity atmosphereEntity = entt::null;

        const auto atmosphereView = registry.view<AtmosphereComponent>();
        for (const auto entity : atmosphereView)
        {
            atmosphere = &atmosphereView.get<AtmosphereComponent>(entity);
            atmosphereEntity = entity;
            break;
        }

        if (!atmosphere) return false;

        // Every length in the model is stretched by the exaggeration and every coefficient divided by
        // it. Optical depth is density times distance, so the product is untouched and the sky keeps
        // its colour -- the zenith is exactly as blue as the authored numbers say. What changes is how
        // much of the planet's radius the shell takes up, which is the whole point. See
        // AtmosphereComponent::ThicknessExaggeration.
        const float thickness = Math::Max(atmosphere->ThicknessExaggeration, 0.01f);
        const float density = 1.0f / thickness;

        m_Data.RayleighScattering = atmosphere->RayleighScattering * density;
        m_Data.RayleighScaleHeight = Math::Max(atmosphere->RayleighScaleHeight * thickness, Math::FloatEpsilon);

        m_Data.AbsorptionCoefficients = atmosphere->AbsorptionCoefficients * density;
        m_Data.MieScattering = atmosphere->MieScattering * density;
        m_Data.MieAbsorption = atmosphere->MieAbsorption * density;
        m_Data.MieScaleHeight = Math::Max(atmosphere->MieScaleHeight * thickness, Math::FloatEpsilon);
        m_Data.MieAnisotropy = Math::Clamp(atmosphere->MieAnisotropy, -0.99f, 0.99f);

        m_Data.PlanetRadius = atmosphere->PlanetRadius;
        m_Data.AtmosphereRadius = atmosphere->GetAtmosphereRadius();
        m_Data.AbsorptionLayerCenter = atmosphere->AbsorptionLayerCenter * thickness;
        m_Data.AbsorptionLayerWidth = Math::Max(atmosphere->AbsorptionLayerWidth * thickness,
                                                Math::FloatEpsilon);
        m_Data.SkyIntensity = atmosphere->SkyIntensity;

        m_Data.GroundColor = static_cast<Vector3>(atmosphere->GroundColor);
        m_Data.RenderSunDisks = atmosphere->RenderSunDisks ? 1.0f : 0.0f;

        CollectGeometry(registry, params, atmosphereEntity, *atmosphere);

        m_Data.RenderStars = atmosphere->RenderStars ? 1.0f : 0.0f;
        m_Data.StarBrightness = Math::Max(atmosphere->StarBrightness, 0.0f);
        m_Data.StarDensity = Math::Max(atmosphere->StarDensity, 0.0f);
        m_Data.StarTwinkle = Math::Max(atmosphere->StarTwinkle, 0.0f);
        m_Data.MilkyWayBrightness = Math::Max(atmosphere->MilkyWayBrightness, 0.0f);
        m_Data.SkyTime = Time::GetTimeSinceStartup().GetSeconds();

        CollectStarRotation(registry, *atmosphere);

        m_SkyLightUpdateInterval = Math::Max(atmosphere->SkyLightUpdateInterval, 0.0f);
        m_SkyLightUpdateCosAngle = std::cos(Math::Max(atmosphere->SkyLightUpdateAngle, 0.0f) * Math::DegToRad);

        // Cleared rather than left over: HasChangedSince compares the whole array, so a body that
        // was removed has to disappear from the buffer as well.
        for (auto& body : m_Data.Bodies)
            body = GPUAtmosphereBody{};

        uint32_t bodyCount = 0;

        // Stars first: they are what the moons reflect, and a scene that runs out of slots is better
        // off dropping a moon than the sun.
        const auto suns = registry.view<Transform, SunComponent>();
        for (const auto entity : suns)
        {
            if (bodyCount >= GPUAtmosphereData::MaxBodies) break;

            auto [transform, sun] = suns.get<Transform, SunComponent>(entity);

            GPUAtmosphereBody& body = m_Data.Bodies[bodyCount];
            body.Direction = -transform.GetForwardDirection();
            body.Intensity = sun.Intensity;
            body.Color = static_cast<Vector3>(sun.Color);
            body.CosAngularRadius = std::cos(Math::Max(sun.AngularDiameter, 0.0f) * 0.5f * Math::DegToRad);
            body.DiskBrightness = 1.0f;

            bodyCount++;
        }

        const auto moons = registry.view<Transform, MoonComponent>();
        for (const auto entity : moons)
        {
            if (bodyCount >= GPUAtmosphereData::MaxBodies) break;

            auto [transform, moon] = moons.get<Transform, MoonComponent>(entity);

            const Vector3 directionToMoon = -transform.GetForwardDirection();
            const MoonLighting::Illumination illumination = MoonLighting::Collect(registry, moon, directionToMoon);

            // Nothing shines on it, so there is nothing to put in the sky.
            if (illumination.PhaseFactor <= 0.0f) continue;

            GPUAtmosphereBody& body = m_Data.Bodies[bodyCount];
            body.Direction = directionToMoon;
            body.Intensity = 1.0f;
            body.Color = illumination.Radiance;
            body.CosAngularRadius = std::cos(Math::Max(moon.AngularDiameter, 0.0f) * 0.5f * Math::DegToRad);

            // Colour stays at full illumination and the phase is kept apart from it, because the two
            // are used differently: the scattering takes the disk as a whole and scales it by the
            // phase, while the disk itself is shaded per pixel to get the shape of the crescent.
            body.DirectionToStar = illumination.DirectionToStar;
            body.PhaseFactor = illumination.PhaseFactor;
            body.PhaseInfluence = Math::Clamp(moon.PhaseInfluence, 0.0f, 1.0f);
            body.DiskBrightness = Math::Max(moon.DiskBrightness, 0.0f);

            bodyCount++;
        }

        m_Data.BodyCount = bodyCount;
        m_Active = true;

        m_ConstantBuffer->SetData(&m_Data);

        return true;
    }

    void AtmosphereConstants::CollectStarRotation(entt::registry& registry,
                                                  const AtmosphereComponent& atmosphere)
    {
        // Stars share the pole the suns circle, because it is the planet's spin that carries both, but
        // they do not share the rate. A planet comes back round to the same star a little sooner than
        // it comes back round to its own sun, since the sun has moved along its orbit in the meantime;
        // the two rates differ by exactly one turn per year. So the sky is wound forward by that much,
        // which is what walks the constellations away from the sun as the days pass.
        const auto orbits = registry.view<SunComponent, CelestialOrbitComponent>();
        for (const auto entity : orbits)
        {
            const auto& orbit = orbits.get<CelestialOrbitComponent>(entity);

            const float solarTurns = orbit.DaysElapsed + orbit.TimeOfDay;
            const float yearLength = Math::Max(atmosphere.StarYearLength, 1.0f);

            m_Data.StarPole = orbit.GetOrbitPole();
            m_Data.StarRotation = solarTurns * (1.0f + 1.0f / yearLength) * Math::Pi2
                * Math::Max(atmosphere.StarRotationSpeed, 0.0f);

            return;
        }

        // No sun on an orbit, so there is nothing to keep station with. A pole tilted halfway between
        // the zenith and the horizon is what a middle latitude looks like, and the sky holds still.
        m_Data.StarPole = Vector3{0.0f, 0.70710678f, 0.70710678f};
        m_Data.StarRotation = 0.0f;
    }

    void AtmosphereConstants::Bind() const
    {
        if (!m_Active) return;

        m_ConstantBuffer->Bind(ShaderType::Pixel);
    }

    void AtmosphereConstants::CollectGeometry(entt::registry& registry, const RenderParams& params,
                                             const entt::entity entity, const AtmosphereComponent& atmosphere)
    {
        // A planet on the same entity is the air's own planet: its centre is the entity's position and
        // its radius is the one the ground was generated at, so the shell wraps the ground exactly.
        const auto* planet = registry.try_get<PlanetComponent>(entity);
        const auto* transform = registry.try_get<Transform>(entity);

        if (planet && transform)
        {
            const float worldRadius = Math::Max(planet->Shape.Radius, Math::FloatEpsilon);

            m_Data.PlanetCenterWorld = transform->GetWorldPosition();
            m_Data.AtmosphereScale = atmosphere.PlanetRadius / worldRadius;
            m_Data.AtmosphereCameraPosition =
                (params.CameraPosition - m_Data.PlanetCenterWorld) * m_Data.AtmosphereScale;

            return;
        }

        // No planet, so the old flat world: a ground plane at SeaLevelHeight with a planet's curvature
        // borrowed for the horizon. That is the same thing as a planet whose centre sits one radius
        // below you, which is how it is expressed here -- so a scene with a sky and no planet renders
        // exactly what it did before, and there is only one path through the shader.
        m_Data.AtmosphereScale = atmosphere.UnitsToKilometers;

        const float radiusInUnits = atmosphere.PlanetRadius
            / Math::Max(atmosphere.UnitsToKilometers, Math::FloatEpsilon);

        m_Data.PlanetCenterWorld = Vector3(params.CameraPosition.x,
                                           atmosphere.SeaLevelHeight - radiusInUnits,
                                           params.CameraPosition.z);

        const float height = (params.CameraPosition.y - atmosphere.SeaLevelHeight)
            * atmosphere.UnitsToKilometers;

        m_Data.AtmosphereCameraPosition = Vector3(0.0f, atmosphere.PlanetRadius + height, 0.0f);
    }

    bool AtmosphereConstants::HasChangedSince(const GPUAtmosphereData& previous,
                                              const float cosAngleThreshold) const
    {
        // Everything that is close enough to the snapshot is copied from it, so what is left for
        // the comparison is only the fields that actually moved.
        GPUAtmosphereData candidate = m_Data;

        // Only the camera's height off the ground changes what a baked sky looks like; moving sideways
        // over a sphere gives the same sky, and the centre moves with the camera in the flat world case.
        const float height = candidate.AtmosphereCameraPosition.Length();
        const float previousHeight = previous.AtmosphereCameraPosition.Length();

        const float altitude = Math::Max(height - candidate.PlanetRadius, 0.0f);
        const float shellThickness = Math::Max(candidate.AtmosphereRadius - candidate.PlanetRadius,
                                               Math::FloatEpsilon);

        const float tolerance = Math::Max(altitude, shellThickness) * k_ViewHeightTolerance;

        if (std::abs(height - previousHeight) <= tolerance)
        {
            candidate.AtmosphereCameraPosition = previous.AtmosphereCameraPosition;
            candidate.PlanetCenterWorld = previous.PlanetCenterWorld;
        }

        // The star field is drawn straight to the screen and never baked, so nothing the snapshots
        // guard depends on it. Left in the comparison it would report a change every single frame,
        // since the clock it twinkles by never stops.
        candidate.StarPole = previous.StarPole;
        candidate.StarRotation = previous.StarRotation;
        candidate.SkyTime = previous.SkyTime;

        if (candidate.BodyCount == previous.BodyCount)
        {
            // A phase is just an angle in disguise, so it gets the same tolerance the directions do.
            const float phaseThreshold = 0.5f * std::acos(Math::Clamp(cosAngleThreshold, -1.0f, 1.0f));

            for (uint32_t i = 0; i < candidate.BodyCount; ++i)
            {
                GPUAtmosphereBody& body = candidate.Bodies[i];
                const GPUAtmosphereBody& previousBody = previous.Bodies[i];

                if (body.Direction.Dot(previousBody.Direction) >= cosAngleThreshold)
                    body.Direction = previousBody.Direction;

                if (body.DirectionToStar.Dot(previousBody.DirectionToStar) >= cosAngleThreshold)
                    body.DirectionToStar = previousBody.DirectionToStar;

                if (std::abs(body.PhaseFactor - previousBody.PhaseFactor) <= phaseThreshold)
                    body.PhaseFactor = previousBody.PhaseFactor;
            }
        }

        return std::memcmp(&candidate, &previous, sizeof(GPUAtmosphereData)) != 0;
    }
}
