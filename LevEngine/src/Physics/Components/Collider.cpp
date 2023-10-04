#include "levpch.h"
#include "Collider.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	BoxCollider::BoxCollider() = default;
	SphereCollider::SphereCollider() = default;

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
