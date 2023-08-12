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
			DrawMesh(component.mesh);
			DrawMaterial(component.material);

			ImGui::Checkbox("Cast shadows", &component.castShadow);
		}

	private:
		static void DrawMesh(Ref<Mesh>& mesh)
		{
			ImGui::Text("Mesh");
			ImGui::SameLine();
			ImGui::Text(mesh ? mesh->GetName().c_str() : "None. Drop here");
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
					mesh = ObjLoader().LoadMesh(texturePath);
				}
				ImGui::EndDragDropTarget();
			}
		}

		static void DrawMaterial(Material& material)
		{
			if (ImGui::TreeNode("Material"))
			{
				GUIUtils::DrawColor3Control("Ambient", BindGetter(&Material::GetAmbientColor, &material), BindSetter(&Material::SetAmbientColor, &material));
				GUIUtils::DrawColor3Control("Diffuse", BindGetter(&Material::GetDiffuseColor, &material), BindSetter(&Material::SetDiffuseColor, &material));
				GUIUtils::DrawColor3Control("Specular", BindGetter(&Material::GetSpecularColor, &material), BindSetter(&Material::SetSpecularColor, &material));
				GUIUtils::DrawColor3Control("Emissive", BindGetter(&Material::GetEmissiveColor, &material), BindSetter(&Material::SetEmissiveColor, &material));

				GUIUtils::DrawFloatControl("Shininess", BindGetter(&Material::GetShininess, &material), BindSetter(&Material::SetShininess, &material));

				DrawMaterialTexture("Ambient", material, Material::TextureType::Ambient);
				DrawMaterialTexture("Diffuse", material, Material::TextureType::Diffuse);
				DrawMaterialTexture("Specular", material, Material::TextureType::Specular);
				DrawMaterialTexture("Normal", material, Material::TextureType::Normal);
				DrawMaterialTexture("Emissive", material, Material::TextureType::Emissive);

				ImGui::TreePop();
			}
		}

		static void DrawMaterialTexture(const std::string& label, Material& material, Material::TextureType textureType)
		{
			ImGui::PushID(static_cast<int>(textureType));
			ImGui::BeginTable("##texture_props", 2);
			
			ImGui::TableNextColumn();
			ImGui::BeginGroup();
			{
				ImGui::Text(label.c_str());

				auto tiling = material.GetTextureTiling(textureType);
				if (GUIUtils::DrawVector2Control("Tiling", tiling, 1))
					material.SetTextureTiling(textureType, tiling);

				auto offset = material.GetTextureOffset(textureType);
				if (GUIUtils::DrawVector2Control("Offset", offset))
					material.SetTextureOffset(textureType, offset);
			}
			ImGui::EndGroup();
			const ImVec2 size = ImGui::GetItemRectSize();
			
			ImGui::TableNextColumn();

			GUIUtils::DrawTexture2D([&material, textureType] {return material.GetTexture(textureType); },
			                        [&material, textureType](const Ref<Texture>& texture) { material.SetTexture(textureType, texture); }, Vector2(size.y , size.y)); 

			ImGui::EndTable();
			ImGui::PopID();
		}
	};
}
