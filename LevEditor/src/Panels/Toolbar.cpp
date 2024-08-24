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
        DrawSceneStateButton();
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
