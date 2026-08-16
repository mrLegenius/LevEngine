#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class ConstantBuffer;
    struct AtmosphereComponent;
    struct RenderParams;

    // Mirrors AtmosphereBody in Atmosphere.hlsli. A star or a moon -- the sky does not care which,
    // it only ever asks a body for a direction and a radiance.
    struct LEV_API GPUAtmosphereBody
    {
        Vector3 Direction{0.0f, 1.0f, 0.0f};
        float Intensity = 0.0f;

        Vector3 Color{1.0f, 1.0f, 1.0f};
        float CosAngularRadius = 1.0f;

        // Reflective bodies only: towards the star lighting them, which is the axis the crescent is
        // shaded along. Zero for a star, which has no phase.
        Vector3 DirectionToStar{};
        float PhaseFactor = 1.0f;

        // Weight of that shading, mirroring MoonComponent::PhaseInfluence.
        float PhaseInfluence = 0.0f;

        // Exposure of the disk on its own, which a moon needs and a star leaves at one.
        float DiskBrightness = 1.0f;
        Vector2 Padding{};
    };

    // Mirrors AtmosphereConstantBuffer in Atmosphere.hlsli. Keep the two in step.
    struct LEV_API alignas(16) GPUAtmosphereData
    {
        // Stars and moons share these slots, stars first.
        static constexpr uint32_t MaxBodies = 4;

        Vector3 RayleighScattering{};
        float RayleighScaleHeight = 1.0f;

        Vector3 AbsorptionCoefficients{};
        float MieScattering = 0.0f;

        float MieAbsorption = 0.0f;
        float MieScaleHeight = 1.0f;
        float MieAnisotropy = 0.0f;
        float PlanetRadius = 1.0f;

        float AtmosphereRadius = 1.0f;
        float AbsorptionLayerCenter = 0.0f;
        float AbsorptionLayerWidth = 1.0f;
        float SkyIntensity = 1.0f;

        Vector3 GroundColor{};

        // World units to the atmosphere's own units, which are the kilometres every coefficient above
        // is defined in. A planet a hundred units across gets an Earthlike sky because the scattering
        // is run in a space where it is Earth sized -- honest kilometres on a game sized planet come
        // to two kilometres of air and nothing visible.
        float AtmosphereScale = 1.0f;

        // The camera, and the planet's centre, both in the atmosphere's own space: centred on the
        // planet and scaled by the above. This is what replaced a height above a flat sea level -- an
        // atmosphere seen from outside needs to know where the planet is, not just how high you are.
        Vector3 AtmosphereCameraPosition{};
        float CameraPadding = 0.0f;

        //<--- The same centre in world space, so a pixel's depth can be brought into this space ---<<
        Vector3 PlanetCenterWorld{};
        float CenterPadding = 0.0f;

        uint32_t BodyCount = 0;
        float RenderSunDisks = 1.0f;
        Vector2 Padding{};

        // Celestial pole the star field turns about, and how far round it has turned.
        Vector3 StarPole{0.0f, 0.0f, 1.0f};
        float StarRotation = 0.0f;

        float RenderStars = 1.0f;
        float StarBrightness = 1.0f;
        float StarDensity = 1.0f;
        float StarTwinkle = 0.0f;

        float MilkyWayBrightness = 0.0f;

        // Only the twinkling reads this, and nothing baked into a cubemap does, so it is exempt from
        // the change comparison below.
        float SkyTime = 0.0f;
        Vector2 StarPadding{};

        GPUAtmosphereBody Bodies[MaxBodies]{};
    };

    // Gathers the scene's planet, its stars and its moons into the constant buffer both sky passes read.
    // Also answers whether the image based lighting built from this sky is still current, since
    // rebuilding it costs far more than a frame's worth of sky.
    class LEV_API AtmosphereConstants
    {
    public:
        AtmosphereConstants();

        // Returns whether the scene has an atmosphere to render. Data is only valid when it does.
        bool Collect(entt::registry& registry, const RenderParams& params);

        void Bind() const;

        [[nodiscard]] bool IsActive() const { return m_Active; }
        [[nodiscard]] const GPUAtmosphereData& GetData() const { return m_Data; }

        [[nodiscard]] float GetSkyLightUpdateInterval() const { return m_SkyLightUpdateInterval; }
        [[nodiscard]] float GetSkyLightUpdateCosAngle() const { return m_SkyLightUpdateCosAngle; }

        // True when the sky has changed since the given snapshot, treating bodies that moved by
        // less than the given angle (as a cosine) as unchanged.
        [[nodiscard]] bool HasChangedSince(const GPUAtmosphereData& previous, float cosAngleThreshold) const;

    private:
        void CollectStarRotation(entt::registry& registry, const AtmosphereComponent& atmosphere);

        //<--- Where the planet is, in world space and in the atmosphere's own space ---<<
        void CollectGeometry(entt::registry& registry, const RenderParams& params,
                             entt::entity entity, const AtmosphereComponent& atmosphere);

        GPUAtmosphereData m_Data{};
        bool m_Active = false;

        float m_SkyLightUpdateInterval = 0.1f;
        float m_SkyLightUpdateCosAngle = 1.0f;

        Ref<ConstantBuffer> m_ConstantBuffer;
    };
}
