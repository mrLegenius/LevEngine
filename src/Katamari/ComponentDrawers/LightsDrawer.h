#pragma once
#include "ComponentDrawer.h"

namespace LevEngine
{
	class DirectionalLightDrawer final : public ComponentDrawer<DirectionalLightComponent>
	{
	protected:
		std::string GetLabel() const override { return "Directional Light"; }

		void DrawContent(DirectionalLightComponent& component) override
		{
			ImGui::ColorEdit3("Color", component.color.Raw());
		}
	};

	class PointLightDrawer final : public ComponentDrawer<PointLightComponent>
	{
	protected:
		std::string GetLabel() const override { return "Point Light"; }

		void DrawContent(PointLightComponent& component) override
		{
			ImGui::ColorEdit3("Color", component.color.Raw());

			ImGui::DragFloat("Range", &component.Range);
			ImGui::DragFloat("Intensity", &component.Intensity);
			ImGui::DragFloat("Smoothness", &component.Smoothness);
		}
	};
}
