#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class EmitterDrawer final : public ComponentDrawer<EmitterComponent, EmitterDrawer>
	{
	protected:
		[[nodiscard]] String GetLabel() const override { return "Emitter"; }

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
			ImGui::ColorEdit4("Initial Color", component.Birth.StartColor.Raw());
			if (component.Birth.RandomStartColor)
				ImGui::ColorEdit4("Initial Color B", component.Birth.StartColorB.Raw());
			ImGui::ColorEdit4("End Color", component.Birth.EndColor.Raw());

			ImGui::Checkbox("Random Initial Size", &component.Birth.RandomStartSize);
			ImGui::DragFloat("Initial Size", &component.Birth.StartSize, 0.1, 0, std::numeric_limits<float>::max());
			if (component.Birth.RandomStartSize)
				ImGui::DragFloat("Initial Size B", &component.Birth.StartSizeB, 0.1, 0, std::numeric_limits<float>::max());
			ImGui::DragFloat("End Size", &component.Birth.EndSize, 0.1, 0, std::numeric_limits<float>::max());

			ImGui::Checkbox("Random Initial Life Time", &component.Birth.RandomStartLifeTime);
			ImGui::DragFloat("Initial Life Time", &component.Birth.LifeTime, 0.1, 0, std::numeric_limits<float>::max());
			if (component.Birth.RandomStartLifeTime)
				ImGui::DragFloat("Initial Life Time B", &component.Birth.LifeTimeB, 0.1, 0, std::numeric_limits<float>::max());

			ImGui::DragFloat("Gravity Scale", &component.Birth.GravityScale);

			int maxParticles = static_cast<int>(component.MaxParticles);
			if (ImGui::DragInt("Max Particles", &maxParticles, 1, 1, std::numeric_limits<int>::max()))
				component.MaxParticles = maxParticles;
			ImGui::DragFloat("Spawn Rate", &component.Rate, 0.1, 0, std::numeric_limits<float>::max());

			GUIUtils::DrawTextureAsset("Texture", &component.Texture);
		}
	};
}
