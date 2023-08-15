#include "pch.h"
#include "Transform.h"

#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"

void Transform::SetParent(Transform* value, const bool keepWorldTransform)
{
	if (parent == value) return;

	if (parent)
		parent->children.erase(this);

	if (value)
		value->children.insert(this);

	if (!keepWorldTransform)
	{
		parent = value;
		return;
	}

	const auto position = GetWorldPosition();
	const auto rotation = GetWorldOrientation();
	const auto scale = GetWorldScale();

	parent = value;

	SetWorldPosition(position);
	SetWorldRotation(rotation);
	SetWorldScale(scale);
}

class TransformDrawer final : public ComponentDrawer<Transform, TransformDrawer>
{
protected:
	std::string GetLabel() const override { return "Transform"; }
	bool IsRemovable() const override { return false; }

	void DrawContent(Transform& component) override
	{
		auto position = component.GetLocalPosition();
		if (GUIUtils::DrawVector3Control("Position", position))
			component.SetLocalPosition(position);

		Vector3 rotation = Math::ToDegrees(component.GetLocalRotation());
		if (GUIUtils::DrawVector3Control("Rotation", rotation))
			component.SetLocalRotationRadians(Math::ToRadians(rotation));

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
