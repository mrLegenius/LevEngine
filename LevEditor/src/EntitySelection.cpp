#include "pch.h"
#include "EntitySelection.h"

#include <imgui.h>

#include "GUI/Drawers/Components/ComponentDrawer.h"
#include "Undo/UndoCommands.h"

namespace LevEngine::Editor
{
	void EntitySelection::SelectEntity(const Entity entity)
	{
		if (const auto entitySelection = Selection::CurrentAs<EntitySelection>())
			entitySelection->Set(entity);
		else
			Selection::Select(CreateRef<EntitySelection>(entity));
	}

	void EntitySelection::DrawProperties()
	{
		if (m_Entity.HasComponent<TagComponent>())
		{
			const Entity entity = m_Entity;
			const auto& tag = entity.GetComponent<TagComponent>().tag;

			EditorGUI::DrawTextInputField("##Tag", tag, [entity](const String& newValue)
			{
				ScopedEntityEdit edit{ entity, Format("Rename {0}", entity.GetName()) };
				entity.GetComponent<TagComponent>().tag = newValue;
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
