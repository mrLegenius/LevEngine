#include "pch.h"
#include "HierarchyPanel.h"

#include <imgui.h>

#include "Scene/Scene.h"

namespace LevEngine::Editor
{
	HierarchyPanel::HierarchyPanel(const Ref<Scene>& scene, const Ref<EntitySelection>& selection) : m_Context(scene), m_Selection(selection) { }

	void HierarchyPanel::DrawContent()
	{
		m_Context->ForEachEntity(
			[&](const Entity entity)
			{
				DrawEntityNode(entity);
			});

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			m_Selection->Set({});

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

		const auto flags =
			(m_Selection->Get() == entity ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_SpanAvailWidth;

		const bool opened = ImGui::TreeNodeEx((void*)static_cast<uint32_t>(entity), flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_Selection->Set(entity);
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
			if (m_Selection->Get() == entity)
				m_Selection->Set({});
		}

	}
}

