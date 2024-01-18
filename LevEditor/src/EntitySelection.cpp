#include "pch.h"
#include "EntitySelection.h"

#include <imgui.h>

#include "GUI/Drawers/Components/ComponentDrawer.h"

namespace LevEngine::Editor
{
	void EntitySelection::DrawProperties()
	{
		if (m_Entity.HasComponent<TagComponent>())
		{
			auto& tag = m_Entity.GetComponent<TagComponent>().tag;
			EditorGUI::DrawTextInputField("##Tag", tag, [&tag](const String& newValue)
			{
				tag = newValue;
			});
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		static auto& componentDrawers = OrderedClassCollection<IComponentDrawer>::Instance();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			for (const auto& componentDrawer : componentDrawers)
				componentDrawer.first->DrawAddComponent(m_Entity);

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		for (const auto& componentDrawer : componentDrawers)
			componentDrawer.first->Draw(m_Entity);
	}
}
