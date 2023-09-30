#include "pch.h"
#include "Panel.h"

#include <imgui.h>

namespace LevEngine::Editor
{
	void Panel::Render()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_WindowPadding.x, m_WindowPadding.y));
		m_Active = ImGui::Begin(GetName().c_str());
		if (m_Active)
		{
			m_Window = ImGui::GetCurrentWindow();
			m_Focused = ImGui::IsWindowFocused();
			m_Hovered = ImGui::IsWindowHovered();

			DrawContent();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	const ImGuiPayload* Panel::BeginDragDropTargetWindow(const char* payloadType)
	{
		using namespace ImGui;
		const ImRect innerRect = GetCurrentWindow()->InnerRect;
		if (!BeginDragDropTargetCustom(innerRect, GetID("##WindowBgArea")))
			return nullptr;

		if (const auto payload = AcceptDragDropPayload(payloadType, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			if (payload->IsPreview())
			{
				ImDrawList* drawList = GetForegroundDrawList();
				drawList->AddRectFilled(innerRect.Min, innerRect.Max, GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
				drawList->AddRect(innerRect.Min, innerRect.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
			}
			if (payload->IsDelivery())
				return payload;
		}

		EndDragDropTarget();
		return nullptr;
	}
}
