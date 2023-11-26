#include "levpch.h"
#include "LegacyRigidbody.h"

#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	LegacyRigidbody::LegacyRigidbody() = default;

	class LegacyRigidbodySerializer final : public ComponentSerializer<LegacyRigidbody, LegacyRigidbodySerializer>
	{
	protected:
		const char* GetKey() override { return "Legacy Rigidbody"; }

		void SerializeData(YAML::Emitter& out, const LegacyRigidbody& component) override
		{
			out << YAML::Key << "Body Type" << YAML::Value << static_cast<int>(component.bodyType);
			out << YAML::Key << "Gravity Scale" << YAML::Value << component.gravityScale;
			out << YAML::Key << "Mass" << YAML::Value << component.mass;
			out << YAML::Key << "Elasticity" << YAML::Value << component.elasticity;
			out << YAML::Key << "Damping" << YAML::Value << component.damping;
			out << YAML::Key << "Angular Damping" << YAML::Value << component.angularDamping;
			out << YAML::Key << "Enabled" << YAML::Value << component.enabled;
		}

		void DeserializeData(YAML::Node& node, LegacyRigidbody& component) override
		{
			component.bodyType = static_cast<LegacyBodyType>(node["Body Type"].as<int>());
			component.gravityScale = node["Gravity Scale"].as<float>();
			component.mass = node["Mass"].as<float>();
			component.elasticity = node["Elasticity"].as<float>();
			component.damping = node["Damping"].as<float>();
			component.angularDamping = node["Angular Damping"].as<float>();
			component.enabled = node["Enabled"].as<bool>();
		}
	};
}
