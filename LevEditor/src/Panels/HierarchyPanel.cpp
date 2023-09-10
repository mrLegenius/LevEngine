#include "pch.h"
#include "HierarchyPanel.h"

#include <imgui.h>

#include "EntitySelection.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
	HierarchyPanel::HierarchyPanel(const Ref<Scene>& scene) : m_Context(scene) { }

	void HierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	bool HierarchyPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0) return false;

		if (e.GetKeyCode() == KeyCode::Delete)
		{
		if (const auto& entitySelection = Selection::CurrentAs<EntitySelection>())
		{
			if (entitySelection->Get())
			{
				m_Context->DestroyEntity(entitySelection->Get());
				Selection::Deselect();
			}
		}
		}
		return false;
	}

	void HierarchyPanel::DrawContent()
	{
		LEV_PROFILE_FUNCTION()

		m_Context->ForEachEntity(
			[&](Entity entity)
			{
				if (!entity.GetComponent<Transform>().GetParent())
					DrawEntityNode(entity);
			});

		for (const auto toDelete : m_EntitiesToDelete)
		{
			m_Context->DestroyEntity(toDelete);
			Selection::Deselect();
		}
		m_EntitiesToDelete.clear();

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			Selection::Deselect();

		//Right click on a blank space
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::MenuItem("Create New Entity"))
				m_Context->CreateEntity("New Entity");

			ImGui::EndPopup();
		}
	}

	void HierarchyPanel::DrawEntityNode(Entity entity)
	{
		LEV_PROFILE_FUNCTION()

		const auto& tag = entity.GetComponent<TagComponent>().tag;

		const auto entitySelection = Selection::CurrentAs<EntitySelection>();
		
		auto flags =
			(entitySelection && entitySelection->Get() == entity ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth;

		if (entity.GetComponent<Transform>().GetChildrenCount() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		const bool opened = ImGui::TreeNodeEx((void*)static_cast<uint32_t>(entity), flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			if (entitySelection)
				entitySelection->Set(entity);
			else
				Selection::Select(CreateRef<EntitySelection>(entity));
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload(GUIUtils::EntityPayload, &entity, sizeof(Entity));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIUtils::EntityPayload))
			{
				const auto draggedEntity = *static_cast<Entity*>(payload->Data);

				if (entity != draggedEntity)
				{
					draggedEntity.GetComponent<Transform>().SetParent(entity);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete", "delete"))
				m_EntitiesToDelete.emplace(m_EntitiesToDelete.begin(), entity);
			if (ImGui::MenuItem("Duplicate", "ctrl+D"))
				m_Context->DuplicateEntity(entity);

			ImGui::EndPopup();
		}

		if (opened)
		{
			auto& children = entity.GetComponent<Transform>().GetChildren();

			for (const auto child : children)
			{
				DrawEntityNode(child);
			}
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}
}

