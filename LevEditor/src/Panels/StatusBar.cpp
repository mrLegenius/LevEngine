#include "pch.h"
#include "StatusBar.h"

#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    StatusBar::StatusBar()
    {
        m_Height = 10;
    }

    void StatusBar::Render() const
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, m_Height));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, m_Height));
        ImGui::SetNextWindowViewport(viewport->ID);

        GUI::ScopedVariable windowPadding(ImGuiStyleVar_WindowPadding, Vector2(0, 2));
        GUI::ScopedVariable itemInnerSpacing(ImGuiStyleVar_ItemInnerSpacing, Vector2::Zero);
        GUI::ScopedVariable windowBorderSize(ImGuiStyleVar_WindowBorderSize, 0);
        
        GUI::ScopedColor buttonColor(ImGuiCol_Button, Color::Clear);
        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        GUI::ScopedColor buttonHoveredColor(ImGuiCol_ButtonHovered, Color(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        GUI::ScopedColor buttonActiveColor(ImGuiCol_ButtonActive, Color(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
        
        constexpr ImGuiWindowFlags windowFlags = 0
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("##statusbar", nullptr, windowFlags);
        ImGui::End();
    }
}
