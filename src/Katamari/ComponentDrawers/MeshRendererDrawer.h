#pragma once
#include "Scene/Components/Components.h"
#include "GUI/GUIUtils.h"
#include "Math/Math.h"
#include "ComponentDrawer.h"
#include "Katamari/ObjLoader.h"

namespace LevEngine
{
	class MeshRendererDrawer final : public ComponentDrawer<MeshRendererComponent>
	{
	protected:
		std::string GetLabel() const override { return "Mesh Renderer"; }

		void DrawContent(MeshRendererComponent& component) override
		{
			DrawMesh(component);

			//Draw Material

			ImGui::Checkbox("Cast shadows", &component.castShadow);
		}

	private:
		static void DrawMesh(MeshRendererComponent& component)
		{
			ImGui::Text("Mesh");
			ImGui::SameLine();
			ImGui::Text(component.mesh ? component.mesh->GetName().c_str() : "None. Drop here");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIUtils::AssetPayload))
				{
					const auto path = static_cast<const wchar_t*>(payload->Data);
					const std::filesystem::path texturePath = GUIUtils::AssetsPath / path;

					if (!GUIUtils::IsAssetMesh)
					{
						ImGui::EndDragDropTarget();
						return;
					}
					component.mesh = ObjLoader().LoadMesh(texturePath);
				}
				ImGui::EndDragDropTarget();
			}
		}
	};
}
