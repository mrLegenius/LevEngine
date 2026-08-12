#include "pch.h"
#include "TitleBar.h"

#include "MenuBar.h"
#include "GUI/ScopedGUIHelpers.h"
#include "GUI/Icons/IconsFontAwesome6.h"

namespace LevEngine::Editor
{
    namespace
    {
        constexpr float k_Padding = 8.0f;
        constexpr float k_HeightInFrames = 1.75f;
        constexpr float k_LogoScale = 0.6f;
        constexpr float k_ButtonWidthInHeights = 1.5f;
        const Vector2 k_MenuPopupPadding{ 10, 5 };
    }

    void TitleBar::Render(const Ref<MenuBar>& menuBar, const String& title)
    {
        LEV_PROFILE_FUNCTION();

        auto& window = App::Get().GetWindow();
        if (!window.HasCustomTitleBar()) return;

        m_Height = ImGui::GetFrameHeight() * k_HeightInFrames;

        constexpr ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoSavedSettings;

        const auto& colors = ImGui::GetStyle().Colors;

        GUI::ScopedColor background(ImGuiCol_WindowBg, colors[ImGuiCol_MenuBarBg]);

        //Only the title bar itself is padding free. Popping the padding right after Begin keeps
        //the menu popups opened from here at the default style
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        const bool isTitleBarVisible = ImGui::BeginViewportSideBar("##MainTitleBar", nullptr, ImGuiDir_Up,
                                                                   m_Height, windowFlags);
        ImGui::PopStyleVar();

        bool isHovered = false;

        if (isTitleBarVisible)
        {
            const auto windowPos = ImGui::GetWindowPos();

            float dragZoneStart = DrawLogo();

            if (menuBar)
            {
                const ImRect menuRect
                {
                    ImVec2(dragZoneStart, windowPos.y + (m_Height - ImGui::GetFrameHeight()) * 0.5f),
                    ImVec2(windowPos.x + ImGui::GetWindowWidth(), windowPos.y + m_Height)
                };

                //The same padding the other popups in the editor use
                GUI::ScopedVariable popupPadding(ImGuiStyleVar_WindowPadding, k_MenuPopupPadding);

                dragZoneStart = menuBar->RenderInRect(menuRect) + k_Padding;
            }

            const float dragZoneEnd = windowPos.x + ImGui::GetWindowWidth() - DrawWindowButtons();

            DrawTitle(title, dragZoneStart, dragZoneEnd);

            //Everything in the title bar that is not an item is a drag zone, which the window
            //turns into a caption for the system to drag, snap and maximize
            isHovered = ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered();
        }

        ImGui::End();

        window.SetTitleBarHovered(isHovered);
    }

    float TitleBar::DrawLogo() const
    {
        static const Ref<Texture> logo = Icons::Logo();

        const float logoSize = m_Height * k_LogoScale;

        ImGui::SetCursorPos(ImVec2(k_Padding, (m_Height - logoSize) * 0.5f));
        ImGui::Image(logo->GetId(), ImVec2(logoSize, logoSize));

        return ImGui::GetWindowPos().x + k_Padding * 2 + logoSize;
    }

    void TitleBar::DrawTitle(const String& title, const float dragZoneStart, const float dragZoneEnd) const
    {
        if (title.empty()) return;

        //The first font is the bold one, see ImGuiLayer::OnAttach
        GUI::ScopedFont bold(ImGui::GetIO().Fonts->Fonts[0]);

        const float textWidth = ImGui::CalcTextSize(title.c_str()).x;
        const float zoneCenter = (dragZoneStart + dragZoneEnd) * 0.5f;
        const float textStart = ImMax(dragZoneStart, zoneCenter - textWidth * 0.5f);

        //Rather no title at all than a title running over the menus or the buttons
        if (textStart + textWidth > dragZoneEnd) return;

        ImGui::SetCursorPos(ImVec2(textStart - ImGui::GetWindowPos().x,
                                   (m_Height - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::TextDisabled("%s", title.c_str());
    }

    float TitleBar::DrawWindowButtons() const
    {
        auto& window = App::Get().GetWindow();

        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];

        GUI::ScopedVariable frameRounding(ImGuiStyleVar_FrameRounding, 0.0f);
        GUI::ScopedColor buttonColor(ImGuiCol_Button, Color::Clear);
        GUI::ScopedColor buttonHoveredColor(ImGuiCol_ButtonHovered,
                                            ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));

        const float buttonWidth = m_Height * k_ButtonWidthInHeights;
        const float buttonsWidth = buttonWidth * 3;
        const auto buttonSize = ImVec2(buttonWidth, m_Height);

        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - buttonsWidth, 0.0f));

        if (ImGui::Button(ICON_FA_WINDOW_MINIMIZE, buttonSize))
            window.Minimize();

        ImGui::SameLine(0, 0);

        const bool isMaximized = window.IsMaximized();
        if (ImGui::Button(isMaximized ? ICON_FA_WINDOW_RESTORE : ICON_FA_WINDOW_MAXIMIZE, buttonSize))
        {
            if (isMaximized)
                window.Restore();
            else
                window.Maximize();
        }

        ImGui::SameLine(0, 0);

        {
            GUI::ScopedColor closeHovered(ImGuiCol_ButtonHovered, Color(0.75f, 0.15f, 0.15f, 1.0f));
            GUI::ScopedColor closeActive(ImGuiCol_ButtonActive, Color(0.9f, 0.3f, 0.3f, 1.0f));

            if (ImGui::Button(ICON_FA_XMARK, buttonSize))
                App::Get().Close();
        }

        return buttonsWidth;
    }
}
