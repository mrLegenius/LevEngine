#include "levpch.h"
#include "Rigidbody.h"

#include "GUI/GUIUtils.h"
#include "Scene/Components/ComponentDrawer.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	Rigidbody::Rigidbody() = default;

	class RigidbodyDrawer final : public ComponentDrawer<Rigidbody, RigidbodyDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Rigidbody"; }

		void DrawContent(Rigidbody& component) override
		{
			const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic" };
			const char* currentBodyTypeString = bodyTypeStrings[static_cast<int>(component.bodyType)];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					const bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.bodyType = static_cast<BodyType>(i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			GUIUtils::DrawFloatControl("Gravity Scale", component.gravityScale);
			GUIUtils::DrawFloatControl("Mass", component.mass);
			GUIUtils::DrawFloatControl("Elasticity", component.elasticity);
			GUIUtils::DrawFloatControl("Damping", component.damping);
			GUIUtils::DrawFloatControl("Angular Damping", component.angularDamping);

			ImGui::Checkbox("Enabled", &component.enabled);
		}
	};

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
