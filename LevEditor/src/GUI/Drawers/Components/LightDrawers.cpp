#include "pch.h"

#include "ComponentDrawer.h"

namespace LevEngine::Editor
{
    class DirectionalLightDrawer final : public ComponentDrawer<DirectionalLightComponent, DirectionalLightDrawer>
    {
    protected:
        String GetLabel() const override { return "Lights/Directional Light"; }

        void DrawContent(DirectionalLightComponent& component) override
        {
            ImGui::ColorEdit3("Color", component.color.Raw());
        }
    };

    class PointLightDrawer final : public ComponentDrawer<PointLightComponent, PointLightDrawer>
    {
    protected:
        String GetLabel() const override { return "Lights/Point Light"; }
        
        void DrawContent(PointLightComponent& component) override
        {
            ImGui::ColorEdit3("Color", component.color.Raw());

            ImGui::DragFloat("Range", &component.Range, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Smoothness", &component.Smoothness, 0.1f, 0.0f, 1.0f);
        }
    };

    class SpotLightDrawer final : public ComponentDrawer<SpotLightComponent, SpotLightDrawer>
    {
    protected:
        String GetLabel() const override { return "Lights/Spot Light"; }
        
        void DrawContent(SpotLightComponent& component) override
        {
            ImGui::ColorEdit3("Color", component.color.Raw());

            ImGui::DragFloat("Range", &component.Range, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Angle", &component.Angle, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Smoothness", &component.Smoothness, 0.1f, 0.0f, 1.0f);
        }
    };
}
