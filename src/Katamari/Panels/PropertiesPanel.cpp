#include "pch.h"
#include "PropertiesPanel.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Scene/Entity.h"

namespace LevEngine::Editor
{
void PropertiesPanel::DrawVector3Control(const std::string& label, Vector3& values, const float resetValue = 0.0f, const float columnWidth = 100.0f)
{
	// -- Init -------------------------------------------------------
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	const ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	// -- X Component ------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushFont(boldFont);

	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;

	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// -- Y Component ------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;

	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// -- Z Component ------------------------------------------------
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;

	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f);
	ImGui::PopItemWidth();

	// -- Reset ------------------------------------------------------
	ImGui::PopStyleVar();
	ImGui::Columns();
	ImGui::PopID();
}

template<typename T, typename UIFunction>
void PropertiesPanel::DrawComponent(const std::string& label, Entity entity, UIFunction draw, const bool removable = true)
{
	constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_AllowItemOverlap
		| ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_FramePadding;

	if (!entity.HasComponent<T>()) return;

	auto& component = entity.GetComponent<T>();
	const auto contentRegionAvail = ImGui::GetContentRegionAvail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
	const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImGui::Separator();
	const bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),
	                                      treeNodeFlags, label.c_str());
	ImGui::PopStyleVar();

	ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);

	if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
	{
		ImGui::OpenPopup("ComponentSettings");
	}

	bool removeComponent = false;
	if (ImGui::BeginPopup("ComponentSettings"))
	{
		if (removable && ImGui::MenuItem("Remove component"))
		{
			removeComponent = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (opened)
	{
		draw(component);

		ImGui::TreePop();
	}

	if (removeComponent)
		entity.RemoveComponent<T>();
}

void PropertiesPanel::DrawComponents(Entity entity)
{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().tag;

		char buffer[256];
		memset(buffer, 0, sizeof buffer);
		strcpy_s(buffer, sizeof buffer, tag.c_str());

		if (ImGui::InputText("##Tag", buffer, sizeof buffer))
		{
			tag = std::string(buffer);
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		DrawAddComponent<CameraComponent>("Camera");
		DrawAddComponent<MeshRendererComponent>("Mesh Renderer");
		DrawAddComponent<SkyboxRendererComponent>("Skybox Renderer");
		DrawAddComponent<DirectionalLightComponent>("Directional Light");
		DrawAddComponent<PointLightComponent>("Point Light");

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	DrawComponent<Transform>("Transform", entity, [](Transform& component)
	{
		auto position = component.GetLocalPosition();
		DrawVector3Control("Position", position);
		component.SetLocalPosition(position);

		Vector3 rotation = Math::ToDegrees(component.GetLocalRotation());
		DrawVector3Control("Rotation", rotation);
		component.SetLocalRotationRadians(Math::ToRadians(rotation));

		auto scale = component.GetLocalScale();
		DrawVector3Control("Scale", scale, 1.0f);
		component.SetLocalScale(scale);
	},
	false);

	DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
	{
		auto& camera = component.camera;
		ImGui::Checkbox("Main", &component.isMain);
		ImGui::Checkbox("Fixed aspect ratio", &component.fixedAspectRatio);
		const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
		const char* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(camera.GetProjectionType())];
		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				const bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
		{
			float size = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Orthographic size", &size))
				camera.SetOrthographicSize(size);

			float nearClip = camera.GetOrthographicNear();
			if (ImGui::DragFloat("Near clip", &nearClip))
				camera.SetOrthographicNear(nearClip);

			float farClip = camera.GetOrthographicFar();
			if (ImGui::DragFloat("Far clip", &farClip))
				camera.SetOrthographicFar(farClip);
		}
		else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
		{
			float fov = Math::ToDegrees(camera.GetFieldOfView());
			ImGui::DragFloat("Field of View", &fov);
			camera.SetFieldOfView(fov);

			float nearClip = camera.GetPerspectiveNear();
			ImGui::DragFloat("Near clip", &nearClip);
			camera.SetPerspectiveNear(nearClip);

			float farClip = camera.GetPerspectiveFar();
			ImGui::DragFloat("Far clip", &farClip);
			camera.SetPerspectiveFar(farClip);
	}});

	/*DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component)
	{
		ImGui::Button("Shader", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				auto path = (const wchar_t*)payload->Data;
				std::filesystem::path shaderPath = std::filesystem::path(g_AssetsPath) / path;
				Ref<Shader> shader = Shader::Create(shaderPath.string());

				component.Shader = shader;
			}
			ImGui::EndDragDropTarget();
		}

		DrawTexture2D("Texture", [&component](const Ref<Texture2D>& texture)
		{
			component.Texture = texture;
		});

		if (ImGui::Button("Plane", ImVec2(100.0f, 0.0f)))
		{
			component.Mesh = Mesh::CreatePlane(2);
		}

		static int sphereResolution = 3;
		ImGui::DragInt("Sphere resolution", &sphereResolution, 1, 3, 100);
		if (ImGui::Button("Sphere", ImVec2(100.0f, 0.0f)))
		{
			component.Mesh = Mesh::CreateSphere(sphereResolution);
		}

		if (ImGui::Button("Cube", ImVec2(100.0f, 0.0f)))
		{
			component.Mesh = Mesh::CreateCube();
		}
	});*/

	/*DrawComponent<SkyboxRendererComponent>("Skybox Renderer", entity, [](auto& component)
	{
		ImGui::Button("Skybox textures", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				auto path = (const wchar_t*)payload->Data;
				const std::filesystem::path skyboxDirectory = std::filesystem::path(g_AssetsPath) / path;

				std::string paths[6];
				int i = 0;

				for (auto& directoryEntry : std::filesystem::directory_iterator(skyboxDirectory))
				{
					if (i == 6) break;
					if (directoryEntry.is_directory()) continue;

					const auto& texturePath = directoryEntry.path();
					const std::string filenameString = texturePath.string();

					paths[i] = filenameString;
					i++;
				}

				Ref<Texture> texture = Texture::CreateTextureCube(paths);

				if (texture->IsLoaded())
				{
					component.Texture = texture;
				}
			}
			ImGui::EndDragDropTarget();
		}
	});*/

	/*DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
		{
			ImGui::ColorEdit3("Ambient", glm::value_ptr(component.Ambient));
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.Diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(component.Specular));
		});

	DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
		{
			ImGui::ColorEdit3("Ambient", glm::value_ptr(component.Ambient));
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(component.Diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(component.Specular));

	ImGui::DragFloat("Constant", &component.constant);
	ImGui::DragFloat("Linear", &component.linear);
	ImGui::DragFloat("Quadratic", &component.quadratic);
		});*/
}

void PropertiesPanel::DrawTexture2D(const std::string& label, std::function<void(const Ref<Texture>&)> onTextureLoaded)
{
	/*ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
		{
			auto path = (const wchar_t*)payload->Data;
			std::filesystem::path texturePath = std::filesystem::path(g_AssetsPath) / path;
			Ref<Texture> texture = Texture::Create(texturePath.string());

			if (texture->IsLoaded())
			{
				onTextureLoaded(texture);
			}
			else
			{
				Log::Warning("Could not load texture {0}", texturePath.filename().string());
			}
		}
		ImGui::EndDragDropTarget();
	}*/
}
template<typename T>
void PropertiesPanel::DrawAddComponent(const std::string& label) const
{
	if (!m_Selection->Get().HasComponent<T>())
	{
		if (ImGui::MenuItem(label.c_str()))
		{
			m_Selection->Get().AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}

void PropertiesPanel::DrawContent()
{
	if (m_Selection->Get())
		DrawComponents(m_Selection->Get());
}
}
