#include "levpch.h"
#include "EmitterComponent.h"
#include "Assets/TextureAsset.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	EmitterComponent::EmitterComponent() = default;

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

			SerializeAsset(out, "Texture", component.Texture);
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
			component.Texture = DeserializeAsset<TextureAsset>(node["Texture"]);
			component.MaxParticles = node["MaxParticles"].as<int>();
			component.Rate = node["Rate"].as<float>();
		}
	};
}
