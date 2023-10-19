#include "pch.h"
#include "ModalPopup.h"

#include "GUI/ScopedGUIHelpers.h"

#include "imgui.h"

namespace LevEngine::Editor
{
    void ModalPopup::Show(
        String title,
        String description,
        String buttonText,
        const Action<>& buttonAction)
    {
        auto& popup = Get();
        popup.m_Title = Move(title);
        popup.m_Description = Move(description);
        popup.m_LeftButtonText = Move(buttonText);
        popup.m_LeftButtonAction = buttonAction;
        popup.m_RightButtonText = String();
        popup.m_RightButtonAction = nullptr;

        m_PopupRequested = true;
    }

    void ModalPopup::Show(
        String title,
        String description,
        String leftButtonText,
        String rightButtonText,
        const Action<>& leftButtonAction,
        const Action<>& rightButtonAction)
    {
        auto& popup = Get();
        popup.m_Title = Move(title);
        popup.m_Description = Move(description);
        popup.m_LeftButtonText = Move(leftButtonText);
        popup.m_RightButtonText = Move(rightButtonText);
        popup.m_LeftButtonAction = leftButtonAction;
        popup.m_RightButtonAction = rightButtonAction;

        m_PopupRequested = true;
    }

    void ModalPopup::RenderPopup() const
    {
        if (m_PopupRequested)
        {
            ImGui::OpenPopup(m_Title.c_str());
            m_PopupRequested = false;
        }
        
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoSavedSettings;
        
        GUI::ScopedVariable windowPadding{ImGuiStyleVar_WindowPadding, Vector2{10, 10}};
        if (ImGui::BeginPopupModal(m_Title.c_str(), nullptr, flags))
        {
            ImGui::Text(m_Description.c_str());

            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            
            if (ImGui::Button(m_LeftButtonText.c_str()))
            {
                if (m_LeftButtonAction)
                    m_LeftButtonAction();
                ImGui::CloseCurrentPopup();
            }

            if (!m_RightButtonText.empty())
            {
                ImGui::SameLine();
                if (ImGui::Button(m_RightButtonText.c_str()))
                {
                    if (m_RightButtonAction)
                        m_RightButtonAction();
                
                    ImGui::CloseCurrentPopup();
                }
            }
                
            ImGui::EndPopup();
        }
    }
}
