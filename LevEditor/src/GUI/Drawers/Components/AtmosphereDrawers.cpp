#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Atmosphere/Atmosphere.h"
#include "Scene/Components/Transform/Transform.h"
#include "Scene/Systems/Atmosphere/CelestialOrbitSystem.h"

namespace LevEngine::Editor
{
    class AtmosphereDrawer final : public ComponentDrawer<AtmosphereComponent, AtmosphereDrawer>
    {
    protected:
        String GetLabel() const override { return "Environment/Atmosphere"; }

        void DrawContent(AtmosphereComponent& component) override
        {
            const char* presetNames[] = {"Earth", "Mars", "Titan", "Alien (Green)", "Custom"};

            int preset = static_cast<int>(component.Preset);
            if (ImGui::Combo("Planet", &preset, presetNames, IM_ARRAYSIZE(presetNames)))
                AtmospherePresets::Apply(component, static_cast<AtmospherePreset>(preset));

            ImGui::TextWrapped(
                "Lengths are kilometres and coefficients are extinction rates in 1/km. "
                "Add a Sun (with a Directional Light) to light the planet, and a Moon to keep the "
                "night from being black.");

            // Anything typed by hand stops being one of the presets.
            bool edited = false;

            ImGui::SeparatorText("Geometry");
            edited |= ImGui::DragFloat("Planet Radius", &component.PlanetRadius, 10.0f, 1.0f, 100000.0f);
            edited |= ImGui::DragFloat("Atmosphere Height", &component.AtmosphereHeight, 1.0f, 0.1f, 1000.0f);
            ImGui::DragFloat("Units To Kilometers", &component.UnitsToKilometers, 0.0001f, 0.0f, 1.0f, "%.5f");
            ImGui::DragFloat("Sea Level Height", &component.SeaLevelHeight, 0.1f);

            ImGui::SeparatorText("Rayleigh (air molecules)");
            edited |= ImGui::DragFloat3("Rayleigh Scattering", &component.RayleighScattering.x,
                                        0.0001f, 0.0f, 1.0f, "%.5f");
            edited |= ImGui::DragFloat("Rayleigh Scale Height", &component.RayleighScaleHeight, 0.1f, 0.01f, 200.0f);

            ImGui::SeparatorText("Mie (aerosols and dust)");
            edited |= ImGui::DragFloat("Mie Scattering", &component.MieScattering, 0.0001f, 0.0f, 1.0f, "%.5f");
            edited |= ImGui::DragFloat("Mie Absorption", &component.MieAbsorption, 0.0001f, 0.0f, 1.0f, "%.5f");
            edited |= ImGui::DragFloat("Mie Scale Height", &component.MieScaleHeight, 0.1f, 0.01f, 200.0f);
            edited |= ImGui::DragFloat("Mie Anisotropy", &component.MieAnisotropy, 0.01f, -0.99f, 0.99f);

            ImGui::SeparatorText("Absorption layer (ozone)");
            edited |= ImGui::DragFloat3("Absorption", &component.AbsorptionCoefficients.x,
                                        0.0001f, 0.0f, 1.0f, "%.5f");
            edited |= ImGui::DragFloat("Layer Center", &component.AbsorptionLayerCenter, 0.5f, 0.0f, 500.0f);
            edited |= ImGui::DragFloat("Layer Width", &component.AbsorptionLayerWidth, 0.5f, 0.01f, 500.0f);

            ImGui::SeparatorText("Appearance");
            edited |= ImGui::ColorEdit3("Ground Color", component.GroundColor.Raw());
            ImGui::DragFloat("Sky Intensity", &component.SkyIntensity, 0.01f, 0.0f, 20.0f);
            ImGui::Checkbox("Render Sun and Moon Disks", &component.RenderSunDisks);

            ImGui::SeparatorText("Stars");
            ImGui::Checkbox("Render Stars", &component.RenderStars);
            ImGui::TextWrapped(
                "Hidden by contrast rather than by a switch, so a bright sky buries them however high "
                "the brightness goes, and the air reddens and swallows whatever is low. They turn "
                "about the pole of the first Sun that has a Celestial Orbit.");

            if (component.RenderStars)
            {
                ImGui::DragFloat("Star Brightness", &component.StarBrightness, 0.05f, 0.0f, 20.0f);
                ImGui::DragFloat("Star Density", &component.StarDensity, 0.05f, 0.0f, 20.0f);
                ImGui::DragFloat("Twinkle", &component.StarTwinkle, 0.02f, 0.0f, 4.0f);
                ImGui::DragFloat("Milky Way", &component.MilkyWayBrightness, 0.02f, 0.0f, 10.0f);

                ImGui::DragFloat("Year Length", &component.StarYearLength, 1.0f, 1.0f, 100000.0f);
                ImGui::TextWrapped("Days it takes the sun to work its way once round the stars. Lower "
                                   "it to see the constellations drift between nights sooner.");

                ImGui::DragFloat("Rotation Speed", &component.StarRotationSpeed, 0.01f, 0.0f, 10.0f);
                ImGui::TextWrapped("Turns of the sky per day. 1 keeps step with the sun, which on a "
                                   "short day is a fast sky; 0 nails the stars in place. A longer Day "
                                   "Length slows them without breaking step.");
            }

            ImGui::SeparatorText("Sky lighting");
            ImGui::TextWrapped(
                "The light probe built from this sky is expensive, so it only refreshes when the "
                "sun has moved this far, and no more often than this interval.");
            ImGui::DragFloat("Update Interval", &component.SkyLightUpdateInterval, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Update Angle", &component.SkyLightUpdateAngle, 0.1f, 0.0f, 45.0f);

            if (edited)
                component.Preset = AtmospherePreset::Custom;
        }
    };

    class SunDrawer final : public ComponentDrawer<SunComponent, SunDrawer>
    {
    protected:
        String GetLabel() const override { return "Environment/Sun"; }

        void DrawContent(SunComponent& component) override
        {
            ImGui::ColorEdit3("Color", component.Color.Raw());
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 1000.0f);
            ImGui::DragFloat("Angular Diameter", &component.AngularDiameter, 0.01f, 0.0f, 90.0f);

            ImGui::Separator();
            ImGui::Checkbox("Drives Directional Light", &component.DrivesDirectionalLight);

            if (component.DrivesDirectionalLight)
            {
                ImGui::DragFloat("Light Intensity", &component.LightIntensity, 0.05f, 0.0f, 100.0f);
                ImGui::TextWrapped(
                    "Needs a Directional Light on the same entity. Its colour is overwritten every "
                    "frame with this sun tinted by the atmosphere it shines through.");
            }
        }
    };

    class MoonDrawer final : public ComponentDrawer<MoonComponent, MoonDrawer>
    {
    protected:
        String GetLabel() const override { return "Environment/Moon"; }

        void DrawContent(MoonComponent& component) override
        {
            ImGui::TextWrapped(
                "Shines by reflecting the scene's Suns, so it lights the sky and the scene at night. "
                "Give it a Celestial Orbit offset from the sun's to have it up while the sun is down.");

            ImGui::ColorEdit3("Color", component.Color.Raw());
            ImGui::DragFloat("Reflectance", &component.Reflectance, 0.0001f, 0.0f, 1.0f, "%.5f");
            ImGui::DragFloat("Angular Diameter", &component.AngularDiameter, 0.01f, 0.0f, 90.0f);
            ImGui::DragFloat("Disk Brightness", &component.DiskBrightness, 0.5f, 0.0f, 1000.0f);
            ImGui::TextWrapped("Reflectance is the light it sends -- sky, probe and directional light "
                               "all take it. Disk Brightness is how bright the disk itself reads, and "
                               "nothing else: a real moon outshines every star while lighting the "
                               "ground barely at all, and one number cannot hold both.");
            ImGui::DragFloat("Phase Influence", &component.PhaseInfluence, 0.01f, 0.0f, 1.0f);
            ImGui::TextWrapped("Phase Influence 1 goes from new to full as it orbits; 0 keeps the disk "
                               "fully lit, which is the safe choice when the night must stay lit.");

            ImGui::Separator();
            ImGui::Checkbox("Drives Directional Light", &component.DrivesDirectionalLight);

            if (component.DrivesDirectionalLight)
            {
                ImGui::DragFloat("Light Intensity", &component.LightIntensity, 0.05f, 0.0f, 100.0f);
                ImGui::TextWrapped(
                    "Needs a Directional Light on the same entity. Its colour is overwritten every "
                    "frame with the starlight this moon reflects.");
            }
        }
    };

    class CelestialOrbitDrawer final : public ComponentDrawer<CelestialOrbitComponent, CelestialOrbitDrawer>
    {
    protected:
        String GetLabel() const override { return "Environment/Celestial Orbit"; }

        void DrawContent(CelestialOrbitComponent& component) override { }

        void DrawContent(Entity entity, CelestialOrbitComponent& component) override
        {
            ImGui::Checkbox("Auto Advance", &component.AutoAdvance);
            ImGui::DragFloat("Day Length", &component.DayLength, 1.0f, 0.1f, 100000.0f);

            bool moved = ImGui::SliderFloat("Time Of Day", &component.TimeOfDay, 0.0f, 1.0f);
            ImGui::TextWrapped("0 rises at the horizon, 0.25 is the highest point, 0.5 sets. "
                               "The cycle runs by itself in play mode.");

            ImGui::DragFloat("Days Elapsed", &component.DaysElapsed, 1.0f, 0.0f, 1000000.0f, "%.0f");
            ImGui::TextWrapped("Whole days counted so far. Drag it to move the star field through the "
                               "year without waiting for the nights.");

            ImGui::Separator();
            moved |= ImGui::DragFloat("Inclination", &component.Inclination, 0.5f, -89.0f, 89.0f);
            moved |= ImGui::DragFloat("North Offset", &component.NorthOffset, 1.0f, -360.0f, 360.0f);

            // Systems do not run while editing, so the sky would not follow the slider without this.
            if (moved && entity.HasComponent<Transform>())
                CelestialOrbitSystem::ApplyOrbit(entity.GetComponent<Transform>(), component);
        }
    };
}
