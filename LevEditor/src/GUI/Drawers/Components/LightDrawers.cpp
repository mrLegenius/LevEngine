#include "pch.h"

#include "ComponentDrawer.h"

namespace LevEngine::Editor
{
    class DirectionalLightDrawer final : public ComponentDrawer<DirectionalLightComponent, DirectionalLightDrawer>
    {
    protected:
        String GetLabel() const override { return "Directional Light"; }

        void DrawContent(DirectionalLightComponent& component) override
        {
            ImGui::ColorEdit3("Color", component.color.Raw());
        }
    };

    class PointLightDrawer final : public ComponentDrawer<PointLightComponent, PointLightDrawer>
    {
    protected:
        String GetLabel() const override { return "Point Light"; }

        void DrawContent(PointLightComponent& component) override
        {
            ImGui::ColorEdit3("Color", component.color.Raw());

            ImGui::DragFloat("Range", &component.Range, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, std::numeric_limits<float>::max());
            ImGui::DragFloat("Smoothness", &component.Smoothness, 0.1f, 0.0f, 1.0f);
        }
    };
}
