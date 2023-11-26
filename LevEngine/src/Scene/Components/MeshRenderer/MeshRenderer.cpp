#include "levpch.h"
#include "MeshRenderer.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	MeshRendererComponent::MeshRendererComponent() = default;
	

class MeshRendererSerializer final : public ComponentSerializer<MeshRendererComponent, MeshRendererSerializer>
{
protected:
	const char* GetKey() override { return "MeshRenderer"; }

	void SerializeData(YAML::Emitter& out, const MeshRendererComponent& component) override
	{
		SerializeAsset(out, "Mesh", component.mesh);
		SerializeAsset(out, "Material", component.material);

		out << YAML::Key << "Cast Shadow" << YAML::Value << component.castShadow;
	}

	void DeserializeData(YAML::Node& node, MeshRendererComponent& component) override
	{
		component.mesh = DeserializeAsset<MeshAsset>(node["Mesh"]);
		component.material = DeserializeAsset<MaterialAsset>(node["Material"]);

		component.castShadow = node["Cast Shadow"].as<bool>();
	}
};
}
