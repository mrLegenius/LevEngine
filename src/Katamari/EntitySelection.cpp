#include "pch.h"
#include "EntitySelection.h"

#include <imgui.h>

#include "Kernel/ClassCollection.h"
#include "Scene/Components/ComponentDrawer.h"

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

		static auto& componentDrawers = ClassCollection<IComponentDrawer>::Instance();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			for (const Ref<IComponentDrawer>& componentDrawer : componentDrawers)
				componentDrawer->DrawAddComponent(m_Entity);

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		for (const Ref<IComponentDrawer>& componentDrawer : componentDrawers)
			componentDrawer->Draw(m_Entity);
	}
}
