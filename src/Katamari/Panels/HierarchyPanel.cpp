#include "pch.h"
#include "HierarchyPanel.h"

#include <imgui.h>

#include "Katamari/EntitySelection.h"
#include "Katamari/Selection.h"
#include "Scene/Components/Components.h"

namespace LevEngine::Editor
{
	HierarchyPanel::HierarchyPanel(const Ref<Scene>& scene) : m_Context(scene) { }

	void HierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void HierarchyPanel::DrawContent()
	{
		m_Context->ForEachEntity(
			[&](const Entity entity)
			{
				DrawEntityNode(entity);
			});

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

	void HierarchyPanel::DrawEntityNode(Entity entity) const
	{
		const auto& tag = entity.GetComponent<TagComponent>().tag;

		const auto entitySelection = std::dynamic_pointer_cast<EntitySelection>(Selection::Current());

		const auto flags =
			(entitySelection && entitySelection->Get() == entity ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth;

		const bool opened = ImGui::TreeNodeEx((void*)static_cast<uint32_t>(entity), flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			if (entitySelection)
				entitySelection->Set(entity);
			else
				Selection::Select(CreateRef<EntitySelection>(entity));
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
			Selection::Deselect();
		}
	}
}

