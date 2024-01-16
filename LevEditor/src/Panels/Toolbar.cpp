#include "pch.h"
#include "Toolbar.h"

#include "Gizmo.h"
#include "SceneState.h"
#include "Essentials/MenuBar.h"
#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    Toolbar::Toolbar(
        const Ref<MenuBar>& mainMenuBar,
        const Func<SceneState>& sceneStateGetter,
        const Action<>& sceneStateButtonClicked)
        : m_MainMenuBar(mainMenuBar),
          m_SceneStateGetter(sceneStateGetter),
          m_SceneStateButtonClicked(sceneStateButtonClicked)
    {
        m_Height = 10;
    }

    void Toolbar::Render()
    {
        GUI::ScopedColor buttonColor(ImGuiCol_Button, Color::Clear);
        const auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        GUI::ScopedColor buttonHoveredColor(ImGuiCol_ButtonHovered,
                                            Color(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        GUI::ScopedColor buttonActiveColor(ImGuiCol_ButtonActive,
                                           Color(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        constexpr ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_MenuBar;

        const auto font = ImGui::GetFont();
        const auto originalScale = font->Scale;
        font->Scale = 1.5f;
        ImGui::PushFont(font);
        m_Height = ImGui::GetFrameHeight();

        if (ImGui::BeginViewportSideBar("##Toolbar", nullptr, ImGuiDir_Up, m_Height, windowFlags))
        {
            if (ImGui::BeginMenuBar())
            {
                DrawTools();
                ImGui::EndMenuBar();
            }
        }

        ImGui::End();
        font->Scale = originalScale;
        ImGui::PopFont();
    }

    void Toolbar::DrawTools() const
    {
        DrawViewportControlButtons();
        DrawSceneStateButton();
    }

    void Toolbar::DrawViewportControlButtons() const
    {
        static auto selectIcon = Icons::Select();
        static auto translateIcon = Icons::Translate();
        static auto rotationIcon = Icons::Rotate();
        static auto scaleIcon = Icons::Scale();

        const ImGuiIO& io = ImGui::GetIO();
        const auto boldFont = io.Fonts->Fonts[0];

        const auto padding = ImGui::GetStyle().FramePadding.y * 2;
        const float size = 20;

        ImGui::SetCursorPosX(10);
        ImGui::SetCursorPosY(padding);

        constexpr int columns = 7;
        ImGui::BeginTable("tools", columns, 0, ImVec2(0, 0), size * 4);

        for (int i = 0; i < columns; ++i)
            ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, size);

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_none", Gizmo::Tool == Gizmo::ToolType::None, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::None;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(selectIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_translate", Gizmo::Tool == Gizmo::ToolType::Translate, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::Translate;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(translateIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_rotate", Gizmo::Tool == Gizmo::ToolType::Rotate, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::Rotate;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(rotationIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_scale", Gizmo::Tool == Gizmo::ToolType::Scale, 0, ImVec2(size, size)))
                Gizmo::Tool = Gizmo::ToolType::Scale;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::Image(scaleIcon->GetId(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
        }

        {
            ImGui::TableNextColumn();
            ImGui::Dummy(ImVec2(size, size));
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_space_world", Gizmo::Space == Gizmo::ToolSpace::World, 0, ImVec2(size, size)))
                Gizmo::Space = Gizmo::ToolSpace::World;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);

            ImGui::PushFont(boldFont);

            const auto width = ImGui::GetColumnWidth();
            const auto textWidth = ImGui::CalcTextSize("W").x;
            const auto cursorPosX = cursorPos.x + (width - textWidth) * 0.5f;
            ImGui::SetCursorPosX(cursorPosX);

            ImGui::Text("W");
            ImGui::PopFont();
        }

        {
            ImGui::TableNextColumn();
            const auto cursorPos = ImGui::GetCursorPos();
            if (ImGui::Selectable("##tool_space_local", Gizmo::Space == Gizmo::ToolSpace::Local, 0, ImVec2(size, size)))
                Gizmo::Space = Gizmo::ToolSpace::Local;
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(cursorPos);
            ImGui::PushFont(boldFont);

            const auto width = ImGui::GetColumnWidth();
            const auto textWidth = ImGui::CalcTextSize("L").x;
            const auto cursorPosX = cursorPos.x + (width - textWidth) * 0.5f;
            ImGui::SetCursorPosX(cursorPosX);

            ImGui::Text("L");
            ImGui::PopFont();
        }

        ImGui::EndTable();
    }

    void Toolbar::DrawSceneStateButton() const
    {
        static auto iconPlay = Icons::Play();
        static auto iconStop = Icons::Stop();

        const auto padding = ImGui::GetStyle().FramePadding.y * 2;
        const auto size = m_Height - padding;
        const Ref<Texture> icon = m_SceneStateGetter() == SceneState::Edit ? iconPlay : iconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        ImGui::SetCursorPosY(padding / 2);
        if (ImGui::ImageButton(icon->GetId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            m_SceneStateButtonClicked();
        }
    }
}
