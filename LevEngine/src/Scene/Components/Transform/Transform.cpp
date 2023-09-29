#include "levpch.h"
#include "Transform.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	void Transform::SetParent(Entity value, const bool keepWorldTransform)
	{
		if (entity == value)
		{
			Log::CoreWarning("{0} is trying to set itself as a parent", value.GetName());
			return;
		}

		if (parent == value) return;

		if (parent)
		{
			auto& parentTransform = parent.GetComponent<Transform>();
			parentTransform.RemoveChild(entity);
		}

		if (value)
		{
			auto& newParentTransform = value.GetComponent<Transform>();
			newParentTransform.children.emplace_back(entity);
		}

		if (!keepWorldTransform)
		{
			parent = value;
			return;
		}

		const auto position = GetWorldPosition();
		const auto rotation = GetWorldRotation();
		const auto scale = GetWorldScale();

		parent = value;

		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}

	class TransformSerializer final : public ComponentSerializer<Transform, TransformSerializer>
	{
	protected:
		const char* GetKey() override { return "Transform"; }

		void SerializeData(YAML::Emitter& out, const Transform& component) override
		{
			out << YAML::Key << "Position" << YAML::Value << component.GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << component.GetLocalRotation().ToEuler();
			out << YAML::Key << "Scale" << YAML::Value << component.GetLocalScale();
		}
		void DeserializeData(YAML::Node& node, Transform& component) override
		{
			component.SetLocalPosition(node["Position"].as<Vector3>());
			component.SetLocalRotation(Quaternion::CreateFromYawPitchRoll(node["Rotation"].as<Vector3>()));
			component.SetLocalScale(node["Scale"].as<Vector3>());
		}
	};
}
