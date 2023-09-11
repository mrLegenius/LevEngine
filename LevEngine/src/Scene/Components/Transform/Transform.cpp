#include "levpch.h"
#include "Transform.h"

#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"
#include "GUI/GUIUtils.h"

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

class TransformDrawer final : public ComponentDrawer<Transform, TransformDrawer, -1000>
{
protected:
	String GetLabel() const override { return "Transform"; }
	bool IsRemovable() const override { return false; }

	void DrawContent(Transform& component) override
	{
		auto position = component.GetLocalPosition();
		if (GUIUtils::DrawVector3Control("Position", position))
			component.SetLocalPosition(position);

		Vector3 rotation = Math::ToDegrees(component.GetLocalRotation().ToEuler());
		if (GUIUtils::DrawVector3Control("Rotation", rotation))
			component.SetLocalRotation(Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation)));

		auto scale = component.GetLocalScale();
		if (GUIUtils::DrawVector3Control("Scale", scale, 1.0f))
			component.SetLocalScale(scale);
	}
};

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
