#include "pch.h"
#include "PropertiesPanel.h"

#include <imgui.h>

#include "Components/TransformDrawer.h"
#include "Scene/Entity.h"

namespace LevEngine::Editor
{

	PropertiesPanel::PropertiesPanel(const Ref<EntitySelection>& entitySelection) : m_Selection(entitySelection)
	{
		m_ComponentDrawers = {
			CreateRef<TransformDrawer>(),
		};

	}

	void PropertiesPanel::DrawComponents(Entity entity) const
	{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().tag;

		char buffer[256] = {};
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
		if (m_Selection->Get())
		{
			for (const Ref<IComponentDrawer>& componentDrawer : m_ComponentDrawers)
				componentDrawer->DrawAddComponent(entity);
		}

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	for (const Ref<IComponentDrawer>& componentDrawer : m_ComponentDrawers)
		componentDrawer->Draw(entity);

	/*GUIUtils::DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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
	}});*/

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
