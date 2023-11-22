#include "levpch.h"
#include "LegacyCollider.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	LegacyBoxCollider::LegacyBoxCollider() = default;
	LegacySphereCollider::LegacySphereCollider() = default;

	class LegacySphereColliderSerializer final : public ComponentSerializer<LegacySphereCollider, LegacySphereColliderSerializer>
	{
	protected:
		const char* GetKey() override { return "Legacy Sphere Collider"; }

		void SerializeData(YAML::Emitter& out, const LegacySphereCollider& component) override
		{
			out << YAML::Key << "Radius" << YAML::Value << component.radius;
			out << YAML::Key << "Offset" << YAML::Value << component.offset;
		}

		void DeserializeData(YAML::Node& node, LegacySphereCollider& component) override
		{
			component.radius = node["Radius"].as<float>();
			component.offset = node["Offset"].as<Vector3>();
		}
	};

	class LegacyBoxColliderSerializer final : public ComponentSerializer<LegacyBoxCollider, LegacyBoxColliderSerializer>
	{
	protected:
		const char* GetKey() override { return "Box Collider"; }

		void SerializeData(YAML::Emitter& out, const LegacyBoxCollider& component) override
		{
			out << YAML::Key << "Extents" << YAML::Value << component.extents;
			out << YAML::Key << "Offset" << YAML::Value << component.offset;
		}

		void DeserializeData(YAML::Node& node, LegacyBoxCollider& component) override
		{
			component.extents = node["Extents"].as<Vector3>();
			component.offset = node["Offset"].as<Vector3>();
		}
	};
}
