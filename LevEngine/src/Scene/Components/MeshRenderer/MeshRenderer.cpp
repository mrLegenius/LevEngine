#include "levpch.h"
#include "MeshRenderer.h"

#include "GUI/GUIUtils.h"
#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"
#include "Assets/AssetDatabase.h"

namespace LevEngine
{
	MeshRendererComponent::MeshRendererComponent() = default;

	class MeshRendererDrawer final : public ComponentDrawer<MeshRendererComponent, MeshRendererDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Mesh Renderer"; }

		void DrawContent(MeshRendererComponent& component) override
		{
			DrawMesh(component.mesh);
			DrawMaterial(component.material);

			ImGui::Checkbox("Cast shadows", &component.castShadow);
		}

	private:
		static void DrawMesh(Ref<MeshAsset>& mesh)
		{
			GUIUtils::DrawAsset("Mesh",
				mesh,
				AssetDatabase::IsAssetMesh,
				[&mesh](const auto& path) { mesh = AssetDatabase::GetAsset<MeshAsset>(path); });
		}

		static void DrawMaterial(Ref<MaterialAsset>& material)
		{
			GUIUtils::DrawAsset("Material",
				material,
				AssetDatabase::IsAssetMaterial,
				[&material](const auto& path) { material = AssetDatabase::GetAsset<MaterialAsset>(path);});
		}
	};


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
