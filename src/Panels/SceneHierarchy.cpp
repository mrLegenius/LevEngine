#include "SceneHierarchy.h"
#include <filesystem>

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace LevEngine
{
    extern const std::filesystem::path g_AssetsPath;

	SceneHierarchy::SceneHierarchy(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchy::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = { };
	}

	void SceneHierarchy::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

        if (m_Context)
        {
            m_Context->m_Registry.each([&](auto entityID)
                                       {
                                           const Entity entity{ entityID, m_Context.get() };
                                           DrawEntityNode(entity);
                                       });

            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
                m_SelectionContext = {};

            //Right-click on a blank space
            if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
            {
                if (ImGui::MenuItem("Create New Entity"))
                    m_Context->CreateEntity("New Entity");

                ImGui::EndPopup();
            }
        }
		
		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchy::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().tag;

		const auto flags =
			(m_SelectionContext == entity ? ImGuiTreeNodeFlags_Selected : 0) 
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth;

		const bool opened = ImGui::TreeNodeEx((void*)static_cast<uint32_t>(entity), flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = { };
		}

	}

	static void DrawVector3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
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
	static void DrawComponent(const std::string& label, Entity entity, UIFunction draw, bool removable = true)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			auto contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
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
	}
	
	void SceneHierarchy::DrawComponents(Entity entity)
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

			if(ImGui::BeginPopup("AddComponent"))
			{
                if (!m_SelectionContext.HasComponent<CameraComponent>())
                {
                    if (ImGui::MenuItem("Camera"))
                    {
                        m_SelectionContext.AddComponent<CameraComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }

                if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
                {
                    if (ImGui::MenuItem("Sprite Renderer"))
                    {
                        m_SelectionContext.AddComponent<SpriteRendererComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }

                if (!m_SelectionContext.HasComponent<CircleRendererComponent>())
                {
                    if (ImGui::MenuItem("Circle Renderer"))
                    {
                        m_SelectionContext.AddComponent<CircleRendererComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }
				
				ImGui::EndPopup();
			}
		
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVector3Control("Position", component.position);

				glm::vec3 rotation = glm::degrees(component.rotation);
				DrawVector3Control("Rotation", rotation);
				component.rotation = glm::radians(rotation);

				DrawVector3Control("Scale", component.scale, 1.0f);

			},
			false);
		
		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.camera;
				ImGui::Checkbox("Main", &component.isMain);
				ImGui::Checkbox("Fixed aspect ratio", &component.fixedAspectRatio);
				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(camera.GetProjectionType())];
				if(ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						const bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if(ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
						}

						if(isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					
					ImGui::EndCombo();
				}

				if(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float size = camera.GetOrthographicSize();
					if(ImGui::DragFloat("Orthographic size", &size))
						camera.SetOrthographicSize(size);

					float nearClip = camera.GetOrthographicNear();
					if(ImGui::DragFloat("Near clip", &nearClip))
						camera.SetOrthographicNear(nearClip);

					float farClip = camera.GetOrthographicFar();
					if(ImGui::DragFloat("Far clip", &farClip))
						camera.SetOrthographicFar(farClip);
				}
				else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float fov = glm::degrees(camera.GetFieldOfView());
					ImGui::DragFloat("Field of View", &fov);
					camera.SetFieldOfView(glm::radians(fov));

					float nearClip = camera.GetPerspectiveNear();
					ImGui::DragFloat("Near clip", &nearClip);
					camera.SetPerspectiveNear(nearClip);

					float farClip = camera.GetPerspectiveFar();
					ImGui::DragFloat("Far clip", &farClip);
					camera.SetPerspectiveFar(farClip);
				}
			});
		
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", value_ptr(component.color));

                ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
                    {
                        auto path = (const wchar_t*)payload->Data;
                        std::filesystem::path texturePath = std::filesystem::path(g_AssetsPath) / path;
                        Ref<Texture2D> texture = Texture2D::Create(texturePath.string());

                        if (texture->IsLoaded())
                        {
                            component.Texture = texture;
                        }
                        else
                        {
                            Log::Warning("Could not load texture {0}", texturePath.filename().string());
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});

        DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
            ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
            ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
        });
	}
}
