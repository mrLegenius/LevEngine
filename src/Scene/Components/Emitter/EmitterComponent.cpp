#include "pch.h"
#include "EmitterComponent.h"

#include "TextureLibrary.h"
#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"

namespace LevEngine
{
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
			auto birthProps = node["Birth"];

			birth.RandomVelocity = node["RandomVelocity"].as<bool>();
			birth.Velocity = node["Velocity"].as<Vector3>();
			birth.VelocityB = node["VelocityB"].as<Vector3>();

			birth.RandomStartPosition = node["RandomStartPosition"].as<bool>();
			birth.Position = node["Position"].as<Vector3>();
			birth.PositionB = node["PositionB"].as<Vector3>();

			birth.RandomStartColor = node["RandomStartColor"].as<bool>();
			birth.StartColor = node["StartColor"].as<Color>();
			birth.StartColorB = node["StartColorB"].as<Color>();
			birth.EndColor = node["EndColor"].as<Color>();

			birth.RandomStartSize = node["RandomStartSize"].as<bool>();
			birth.StartSize = node["StartSize"].as<float>();
			birth.StartSizeB = node["StartSizeB"].as<float>();
			birth.EndSize = node["EndSize"].as<float>();

			birth.RandomStartLifeTime = node["RandomStartLifeTime"].as<bool>();
			birth.LifeTime = node["LifeTime"].as<float>();
			birth.LifeTimeB = node["LifeTimeB"].as<float>();

			birth.GravityScale = node["GravityScale"].as<float>();

			component.Texture = TextureLibrary::GetTexture(node["Texture"].as<std::string>());
			component.MaxParticles = node["MaxParticles"].as<int>();
			component.Rate = node["Rate"].as<float>();
		}
	};
}
