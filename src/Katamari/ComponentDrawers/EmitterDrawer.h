#pragma once
#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine
{
	class EmitterDrawer final : public ComponentDrawer<EmitterComponent>
	{
	protected:
		[[nodiscard]] std::string GetLabel() const override { return "Emitter"; }

		void DrawContent(EmitterComponent& component) override
		{
			ImGui::Checkbox("Random Initial Velocity", &component.Birth.RandomVelocity);
			GUIUtils::DrawVector3Control("Initial Velocity", component.Birth.Velocity);
			if (component.Birth.RandomVelocity)
				GUIUtils::DrawVector3Control("Initial Velocity B", component.Birth.VelocityB);

			ImGui::Checkbox("Random Initial Position", &component.Birth.RandomStartPosition);
			GUIUtils::DrawVector3Control("Initial Position", component.Birth.Position);
			if (component.Birth.RandomStartPosition)
				GUIUtils::DrawVector3Control("Initial Position B", component.Birth.PositionB);

			ImGui::Checkbox("Random Initial Color", &component.Birth.RandomStartColor);
			ImGui::ColorEdit3("Initial Color", component.Birth.StartColor.Raw());
			if (component.Birth.RandomStartColor)
				ImGui::ColorEdit3("Initial Color B", component.Birth.StartColorB.Raw());
			ImGui::ColorEdit3("End Color", component.Birth.EndColor.Raw());

			ImGui::Checkbox("Random Initial Size", &component.Birth.RandomStartSize);
			ImGui::DragFloat("Initial Size", &component.Birth.StartSize);
			if (component.Birth.RandomStartSize)
				ImGui::DragFloat("Initial Size B", &component.Birth.StartSizeB);
			ImGui::DragFloat("End Size", &component.Birth.EndSize);

			ImGui::Checkbox("Random Initial Life Time", &component.Birth.RandomStartLifeTime);
			ImGui::DragFloat("Initial Life Time", &component.Birth.LifeTime);
			if (component.Birth.RandomStartLifeTime)
				ImGui::DragFloat("Initial Life Time B", &component.Birth.LifeTimeB);

			ImGui::DragFloat("Gravity Scale", &component.Birth.GravityScale);

			int maxParticles = static_cast<int>(component.MaxParticles);
			if (ImGui::DragInt("Max Particles", &maxParticles))
				component.MaxParticles = maxParticles;
			ImGui::DragFloat("Spawn Rate", &component.Rate);

			GUIUtils::DrawTexture2D("Texture", component.Texture,
				[&component](const Ref<Texture>& texture)
				{
					component.Texture = texture;
				});
		}
	};
}
