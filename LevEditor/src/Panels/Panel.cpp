#include "pch.h"
#include "Panel.h"

#include <imgui.h>

#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
	void Panel::Render()
	{
		LEV_PROFILE_FUNCTION();

		if (m_WindowName.empty())
			m_WindowName = GetName();

		GUI::ScopedVariable padding {ImGuiStyleVar_WindowPadding, m_WindowPadding};

		ImGuiWindowFlags flags = ImGuiWindowFlags_None;
		if (!m_CanScroll)
			flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		ImGui::SetNextWindowSize(ImVec2{ m_DefaultWindowSize.x, m_DefaultWindowSize.y }, ImGuiCond_FirstUseEver);

		m_Active = ImGui::Begin(m_WindowName.c_str(), &m_IsOpen, flags);
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

		//Focus is deferred because it can be requested outside of the GUI rendering
		if (m_FocusRequested)
		{
			m_FocusRequested = false;
			ImGui::SetWindowFocus(m_WindowName.c_str());
		}

		if (m_UnfocusRequested)
		{
			m_UnfocusRequested = false;
			if (m_Focused)
				ImGui::SetWindowFocus(nullptr);
		}
	}

	void Panel::SetInstance(const String& typeName, const int index)
	{
		m_TypeName = typeName;
		m_InstanceIndex = index;
		m_WindowName = Format("{}##{}{}", GetName().c_str(), typeName.c_str(), index);
	}

	void Panel::Focus()
	{
		m_UnfocusRequested = false;
		m_FocusRequested = true;
	}

	void Panel::Unfocus()
	{
		m_FocusRequested = false;
		m_UnfocusRequested = true;
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
