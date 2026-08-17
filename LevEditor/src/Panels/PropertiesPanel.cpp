#include "pch.h"
#include "PropertiesPanel.h"

#include <imgui.h>

#include "EntitySelection.h"
#include "Selection.h"

namespace LevEngine::Editor
{
void PropertiesPanel::DrawContent()
{
	LEV_PROFILE_FUNCTION();

	if (!Selection::Current())
	{
		m_EditTracker.Reset();
		return;
	}

	Selection::Current()->DrawProperties();

	const auto entitySelection = Selection::CurrentAs<EntitySelection>();
	if (!entitySelection)
	{
		m_EditTracker.Reset();
		return;
	}

	//<--- An edit lasts from grabbing a widget to letting it go. A drag and drop from the asset
	//browser holds its source item active instead, which counts just the same ---<<
	const bool interacting = ImGui::IsAnyItemActive()
		&& (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
			|| ImGui::GetDragDropPayload() != nullptr);

	//<--- Pointing at the inspector, so the next frame could be the start of an edit ---<<
	const bool engaged = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows
			| ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
		|| ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	m_EditTracker.Update(entitySelection->Get(), interacting, engaged);
}
}

