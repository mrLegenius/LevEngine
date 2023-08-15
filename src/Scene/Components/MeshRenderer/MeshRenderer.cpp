#include "pch.h"
#include "MeshRenderer.h"

#include <imgui.h>

#include "Assets/MaterialAsset.h"
#include "GUI/GUIUtils.h"
#include "Katamari/ObjLoader.h"
#include "../ComponentDrawer.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
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
		static void DrawMesh(Ref<Mesh>& mesh)
		{
			ImGui::Text("Mesh");
			ImGui::SameLine();
			GUIUtils::DrawAsset(mesh ? mesh->GetName().c_str() : "None. Drop here",
				GUIUtils::IsAssetMesh,
				[&mesh](const auto& path) { mesh = ObjLoader().LoadMesh(path); });
		}

		static void DrawMaterial(Material& material)
		{
			ImGui::Text("Material");
			ImGui::SameLine();
			GUIUtils::DrawAsset("Drop here",
				GUIUtils::IsAssetMaterial,
				[&material](const auto& path) {
					MaterialAsset asset{ path };
					asset.Deserialize();
					material = asset.material;
				});
		}
	};
}

class MeshRendererSerializer final : public ComponentSerializer<MeshRendererComponent, MeshRendererSerializer>
{
protected:
	const char* GetKey() override { return "MeshRenderer"; }

	void SerializeData(YAML::Emitter& out, const MeshRendererComponent& component) override
	{
		//EXAMPLE
		// out << YAML::Key << "Field Name" << YAML::Value << component.field;
	}

	void DeserializeData(YAML::Node& node, MeshRendererComponent& component) override
	{
		//EXAMPLE
		//component.field = node["Field Name"].as<float>();
	}
};
