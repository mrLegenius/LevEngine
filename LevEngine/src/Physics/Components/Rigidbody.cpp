#include "levpch.h"
#include "Rigidbody.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	Rigidbody::Rigidbody() = default;

	class RigidbodySerializer final : public ComponentSerializer<Rigidbody, RigidbodySerializer>
	{
	protected:
		const char* GetKey() override { return "Rigidbody"; }

		void SerializeData(YAML::Emitter& out, const Rigidbody& component) override
		{
			out << YAML::Key << "Body Type" << YAML::Value << static_cast<int>(component.bodyType);
			out << YAML::Key << "Gravity Scale" << YAML::Value << component.gravityScale;
			out << YAML::Key << "Mass" << YAML::Value << component.mass;
			out << YAML::Key << "Elasticity" << YAML::Value << component.elasticity;
			out << YAML::Key << "Damping" << YAML::Value << component.damping;
			out << YAML::Key << "Angular Damping" << YAML::Value << component.angularDamping;
			out << YAML::Key << "Enabled" << YAML::Value << component.enabled;
		}

		void DeserializeData(YAML::Node& node, Rigidbody& component) override
		{
			component.bodyType = static_cast<BodyType>(node["Body Type"].as<int>());
			component.gravityScale = node["Gravity Scale"].as<float>();
			component.mass = node["Mass"].as<float>();
			component.elasticity = node["Elasticity"].as<float>();
			component.damping = node["Damping"].as<float>();
			component.angularDamping = node["Angular Damping"].as<float>();
			component.enabled = node["Enabled"].as<bool>();
		}
	};
}
