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

		static void DrawMaterial(Ref<MaterialAsset>& material)
		{
			ImGui::Text("Material");
			ImGui::SameLine();
			GUIUtils::DrawAsset("Drop here",
				GUIUtils::IsAssetMaterial,
				[&material](const auto& path) {
					const Ref<MaterialAsset> asset = CreateRef<MaterialAsset>(path);
					asset->Deserialize();
					material = asset;
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
		out << YAML::Key << "Mesh" << YAML::Value << component.mesh->GetPath().string();
		out << YAML::Key << "Material" << YAML::Value << component.material->GetPath().string();
		out << YAML::Key << "Cast Shadow" << YAML::Value << component.castShadow;
	}

	void DeserializeData(YAML::Node& node, MeshRendererComponent& component) override
	{
		component.mesh = ObjLoader::LoadMesh(std::filesystem::path(node["Mesh"].as<std::string>()));
		auto materialPath = std::filesystem::path(node["Material"].as<std::string>());

		const Ref<MaterialAsset> materialAsset = CreateRef<MaterialAsset>(materialPath);
		materialAsset->Deserialize();
		component.material = materialAsset;

		component.castShadow = node["Cast Shadow"].as<bool>();
	}
};
