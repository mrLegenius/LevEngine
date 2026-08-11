#pragma once
#include <imgui.h>
#include <imgui_internal.h>

namespace LevEngine::Editor
{
	class Panel
	{
	public:
		virtual ~Panel() = default;
		void Render();

		[[nodiscard]] bool IsFocused() const { return m_Focused; }
		[[nodiscard]] bool IsHovered() const { return m_Hovered; }
		[[nodiscard]] bool IsActive() const { return m_Active; }
		[[nodiscard]] bool IsOpen() const { return m_IsOpen; }

		void Focus();
		void Unfocus();
		void Close() { m_IsOpen = false; }

		// Type name and instance index define the ImGui window id, so they have to be
		// stable between sessions for the layout in imgui.ini to be restored
		void SetInstance(const String& typeName, int index);
		[[nodiscard]] const String& GetTypeName() const { return m_TypeName; }
		[[nodiscard]] int GetInstanceIndex() const { return m_InstanceIndex; }
		[[nodiscard]] const String& GetWindowName() const { return m_WindowName; }

		virtual bool OnKeyPressed(KeyPressedEvent& e) { return false; }
		// Called by PanelManager right before the panel is destroyed
		virtual void OnClose() { }

	protected:
		virtual String GetName() = 0;
		virtual void DrawContent() = 0;
		virtual void OnFocus() const { }
		virtual void OnLostFocus() const { }

		static void* BeginDragDropTargetWindow(const char* payloadType);

		ImGuiWindow* m_Window{};

		bool m_Focused = false;
		bool m_Hovered = false;
		bool m_Active = false;

		bool m_CanScroll = true;

		Vector2 m_WindowPadding{ 10, 5};
		//Size the window opens with when it is not docked and has no saved settings
		Vector2 m_DefaultWindowSize{ 480, 360 };

	private:
		String m_TypeName;
		String m_WindowName;
		int m_InstanceIndex = 0;

		bool m_IsOpen = true;
		bool m_FocusRequested = false;
		bool m_UnfocusRequested = false;
	};
}
