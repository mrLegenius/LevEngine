#include "pch.h"
#include "Panel.h"

#include <imgui.h>

#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
	void Panel::Render()
	{
		GUI::ScopedVariable padding {ImGuiStyleVar_WindowPadding, m_WindowPadding};

		ImGuiWindowFlags flags = ImGuiWindowFlags_None;
		if (!m_CanScroll)
			flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		
		m_Active = ImGui::Begin(GetName().c_str(), nullptr, flags);
		if (m_Active)
		{
			m_Window = ImGui::GetCurrentWindow();

			const bool wasFocused = m_Focused;
			m_Focused = ImGui::IsWindowFocused();
			if (!wasFocused && m_Focused)
				OnFocus();
			else if (wasFocused && !m_Focused)
				OnLostFocus();

			m_Hovered = ImGui::IsWindowHovered();

			DrawContent();
		}
		ImGui::End();
	}

	void Panel::Focus()
	{
		if (m_Focused) return;

		ImGui::FocusWindow(m_Window);
		m_Focused = true;
		OnFocus();
	}

	void Panel::Unfocus()
	{
		if (!m_Focused) return;

		ImGui::FocusWindow(nullptr);
		m_Focused = false;
		OnLostFocus();
	}

	void* Panel::BeginDragDropTargetWindow(const char* payloadType)
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
			{
				const auto data = payload->Data;
				return data;
			}
		}

		EndDragDropTarget();
		return nullptr;
	}
}
