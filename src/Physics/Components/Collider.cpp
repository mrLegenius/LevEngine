#include "pch.h"
#include "Collider.h"

#include "GUI/GUIUtils.h"
#include "Scene/Components/ComponentDrawer.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	class SphereColliderDrawer final : public ComponentDrawer<SphereCollider, SphereColliderDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Sphere Collider"; }

		void DrawContent(SphereCollider& component) override
		{
			GUIUtils::DrawFloatControl("Radius", component.radius);
			GUIUtils::DrawVector3Control("Offset", component.offset);
		}
	};

	class SphereColliderSerializer final : public ComponentSerializer<SphereCollider, SphereColliderSerializer>
	{
	protected:
		const char* GetKey() override { return "Sphere Collider"; }

		void SerializeData(YAML::Emitter& out, const SphereCollider& component) override
		{
			out << YAML::Key << "Radius" << YAML::Value << component.radius;
			out << YAML::Key << "Offset" << YAML::Value << component.offset;
		}

		void DeserializeData(YAML::Node& node, SphereCollider& component) override
		{
			component.radius = node["Radius"].as<float>();
			component.offset = node["Offset"].as<Vector3>();
		}
	};

	class BoxColliderDrawer final : public ComponentDrawer<BoxCollider, BoxColliderDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Box Collider"; }

		void DrawContent(BoxCollider& component) override
		{
			GUIUtils::DrawVector3Control("Extents", component.extents);
			GUIUtils::DrawVector3Control("Offset", component.offset);
		}
	};

	class BoxColliderSerializer final : public ComponentSerializer<BoxCollider, BoxColliderSerializer>
	{
	protected:
		const char* GetKey() override { return "Box Collider"; }

		void SerializeData(YAML::Emitter& out, const BoxCollider& component) override
		{
			out << YAML::Key << "Extents" << YAML::Value << component.extents;
			out << YAML::Key << "Offset" << YAML::Value << component.offset;
		}

		void DeserializeData(YAML::Node& node, BoxCollider& component) override
		{
			component.extents = node["Extents"].as<Vector3>();
			component.offset = node["Offset"].as<Vector3>();
		}
	};
}
