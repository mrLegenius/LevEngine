#pragma once
#include "ComponentSerializer.h"
#include "../SerializerUtils.h"

namespace LevEngine
{
	class TransformSerializer final : public ComponentSerializer<Transform>
	{
	protected:
		const char* GetKey() override { return "Transform"; }

		void SerializeData(YAML::Emitter& out, const Transform& component) override
		{
			out << YAML::Key << "Position" << YAML::Value << component.GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << component.GetLocalRotation();
			out << YAML::Key << "Scale" << YAML::Value << component.GetLocalScale();
		}
		void DeserializeData(YAML::Node& node, Transform& component) override
		{
			component.SetLocalPosition(node["Position"].as<Vector3>());
			component.SetLocalRotationRadians(node["Rotation"].as<Vector3>());
			component.SetLocalScale(node["Scale"].as<Vector3>());
		}
	};
}

