#include "pch.h"
#include "StatusBar.h"

#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    StatusBar::StatusBar()
    {
        m_Height = 10;
    }

    void StatusBar::Render()
    {
        constexpr ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_MenuBar;

        m_Height = ImGui::GetFrameHeight();

        if (ImGui::BeginViewportSideBar("##MainStatusBar", nullptr, ImGuiDir_Down, m_Height, windowFlags))
        {
            if (ImGui::BeginMenuBar())
            {
                ImGui::Text("v0.0.1");
                ImGui::EndMenuBar();
            }
        }

        ImGui::End();
    }
}
