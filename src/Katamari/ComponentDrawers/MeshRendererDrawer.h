#pragma once
#include "Scene/Components/Components.h"
#include "GUI/GUIUtils.h"
#include "Math/Math.h"
#include "ComponentDrawer.h"
#include "Assets/MaterialAsset.h"
#include "Katamari/ObjLoader.h"

namespace LevEngine
{
	class MeshRendererDrawer final : public ComponentDrawer<MeshRendererComponent>
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
					MaterialAsset asset{path};
					asset.Deserialize();
					material = asset.material;
			});
		}
	};
}
