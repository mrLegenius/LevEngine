#include "pch.h"
#include "EmitterComponent.h"

#include "TextureLibrary.h"
#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine
{
	EmitterComponent::EmitterComponent() = default;

	class EmitterDrawer final : public ComponentDrawer<EmitterComponent, EmitterDrawer>
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

			ImGui::Text("Texture");
			GUIUtils::DrawTexture2D(component.Texture);
		}
	};

	class EmitterComponentSerializer final : public ComponentSerializer<EmitterComponent, EmitterComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Emitter"; }

		void SerializeData(YAML::Emitter& out, const EmitterComponent& component) override
		{
			auto& birth = component.Birth;
			out << YAML::Key << "Birth" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "RandomVelocity" << YAML::Value << birth.RandomVelocity;
			out << YAML::Key << "Velocity" << YAML::Value << birth.Velocity;
			out << YAML::Key << "VelocityB" << YAML::Value << birth.VelocityB;

			out << YAML::Key << "RandomStartPosition" << YAML::Value << birth.RandomStartPosition;
			out << YAML::Key << "Position" << YAML::Value << birth.Position;
			out << YAML::Key << "PositionB" << YAML::Value << birth.PositionB;

			out << YAML::Key << "RandomStartColor" << YAML::Value << birth.RandomStartColor;
			out << YAML::Key << "StartColor" << YAML::Value << birth.StartColor;
			out << YAML::Key << "StartColorB" << YAML::Value << birth.StartColorB;
			out << YAML::Key << "EndColor" << YAML::Value << birth.EndColor;

			out << YAML::Key << "RandomStartSize" << YAML::Value << birth.RandomStartSize;
			out << YAML::Key << "StartSize" << YAML::Value << birth.StartSize;
			out << YAML::Key << "StartSizeB" << YAML::Value << birth.StartSizeB;
			out << YAML::Key << "EndSize" << YAML::Value << birth.EndSize;

			out << YAML::Key << "RandomStartLifeTime" << YAML::Value << birth.RandomStartLifeTime;
			out << YAML::Key << "LifeTime" << YAML::Value << birth.LifeTime;
			out << YAML::Key << "LifeTimeB" << YAML::Value << birth.LifeTimeB;

			out << YAML::Key << "GravityScale" << YAML::Value << birth.GravityScale;

			out << YAML::EndMap;

			out << YAML::Key << "Texture" << YAML::Value << component.Texture->GetPath();
			out << YAML::Key << "MaxParticles" << YAML::Value << component.MaxParticles;
			out << YAML::Key << "Rate" << YAML::Value << component.Rate;
		}
		void DeserializeData(YAML::Node& node, EmitterComponent& component) override
		{
			auto& birth = component.Birth;
			const auto birthProps = node["Birth"];

			if (birthProps)
			{
				birth.RandomVelocity = birthProps["RandomVelocity"].as<bool>();
				birth.Velocity = birthProps["Velocity"].as<Vector3>();
				birth.VelocityB = birthProps["VelocityB"].as<Vector3>();

				birth.RandomStartPosition = birthProps["RandomStartPosition"].as<bool>();
				birth.Position = birthProps["Position"].as<Vector3>();
				birth.PositionB = birthProps["PositionB"].as<Vector3>();

				birth.RandomStartColor = birthProps["RandomStartColor"].as<bool>();
				birth.StartColor = birthProps["StartColor"].as<Color>();
				birth.StartColorB = birthProps["StartColorB"].as<Color>();
				birth.EndColor = birthProps["EndColor"].as<Color>();

				birth.RandomStartSize = birthProps["RandomStartSize"].as<bool>();
				birth.StartSize = birthProps["StartSize"].as<float>();
				birth.StartSizeB = birthProps["StartSizeB"].as<float>();
				birth.EndSize = birthProps["EndSize"].as<float>();

				birth.RandomStartLifeTime = birthProps["RandomStartLifeTime"].as<bool>();
				birth.LifeTime = birthProps["LifeTime"].as<float>();
				birth.LifeTimeB = birthProps["LifeTimeB"].as<float>();

				birth.GravityScale = birthProps["GravityScale"].as<float>();
			}
			component.Texture = TextureLibrary::GetTexture(node["Texture"].as<std::string>());
			component.MaxParticles = node["MaxParticles"].as<int>();
			component.Rate = node["Rate"].as<float>();
		}
	};
}
