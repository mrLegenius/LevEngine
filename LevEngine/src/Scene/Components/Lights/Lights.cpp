#include "levpch.h"
#include "Lights.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	DirectionalLightComponent::DirectionalLightComponent() = default;
	PointLightComponent::PointLightComponent() = default;

	class DirectionalLightComponentSerializer final : public ComponentSerializer<DirectionalLightComponent, DirectionalLightComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Directional Light"; }

		void SerializeData(YAML::Emitter& out, const DirectionalLightComponent& component) override
		{
			out << YAML::Key << "Color" << YAML::Value << component.color;
		}

		void DeserializeData(YAML::Node& node, DirectionalLightComponent& component) override
		{
			component.color = node["Color"].as<Color>();
		}
	};

	class PointLightComponentSerializer final : public ComponentSerializer<PointLightComponent, PointLightComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Point Light"; }

		void SerializeData(YAML::Emitter& out, const PointLightComponent& component) override
		{
			out << YAML::Key << "Color" << YAML::Value << component.color;
			out << YAML::Key << "Range" << YAML::Value << component.Range;
			out << YAML::Key << "Smoothness" << YAML::Value << component.Smoothness;
			out << YAML::Key << "Intensity" << YAML::Value << component.Intensity;
		}

		void DeserializeData(YAML::Node& node, PointLightComponent& component) override
		{
			component.color = node["Color"].as<Color>();
			component.Range = node["Range"].as<float>();
			component.Smoothness = node["Smoothness"].as<float>();
			component.Intensity = node["Intensity"].as<float>();
		}
	};


}
