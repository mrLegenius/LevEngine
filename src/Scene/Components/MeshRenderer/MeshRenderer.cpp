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
		static void DrawMesh(Ref<MeshAsset>& mesh)
		{
			GUIUtils::DrawAsset("Mesh",
				mesh,
				GUIUtils::IsAssetMesh,
				[&mesh](const auto& path) { mesh = CreateRef<MeshAsset>(path); });
		}

		static void DrawMaterial(Ref<MaterialAsset>& material)
		{
			GUIUtils::DrawAsset("Material",
				material,
				GUIUtils::IsAssetMaterial,
				[&material](const auto& path) { material = CreateRef<MaterialAsset>(path);});
		}
	};
}

class MeshRendererSerializer final : public ComponentSerializer<MeshRendererComponent, MeshRendererSerializer>
{
protected:
	const char* GetKey() override { return "MeshRenderer"; }

	void SerializeData(YAML::Emitter& out, const MeshRendererComponent& component) override
	{
		if (component.mesh)
			out << YAML::Key << "Mesh" << YAML::Value << component.mesh->GetPath().string();
		out << YAML::Key << "Material" << YAML::Value << component.material->GetPath().string();
		out << YAML::Key << "Cast Shadow" << YAML::Value << component.castShadow;
	}

	void DeserializeData(YAML::Node& node, MeshRendererComponent& component) override
	{
		if (const auto data = node["Mesh"]; data)
		{
			auto assetPath = std::filesystem::path(data.as<std::string>());
			component.mesh = CreateRef<MeshAsset>(assetPath);
		}

		if (const auto data = node["Material"]; data)
		{
			auto assetPath = std::filesystem::path(data.as<std::string>());
			const Ref<MaterialAsset> materialAsset = CreateRef<MaterialAsset>(assetPath);
			materialAsset->Deserialize();
			component.material = materialAsset;
		}

		component.castShadow = node["Cast Shadow"].as<bool>();
	}
};
