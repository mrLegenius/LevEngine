#include "pch.h"
#include "EntitySelection.h"

#include <imgui.h>

namespace LevEngine::Editor
{
	void EntitySelection::DrawProperties()
	{
		if (m_Entity.HasComponent<TagComponent>())
		{
			auto& tag = m_Entity.GetComponent<TagComponent>().tag;

			char buffer[256] = {};
			strcpy_s(buffer, sizeof buffer, tag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof buffer))
			{
				tag = std::string(buffer);
			}
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
