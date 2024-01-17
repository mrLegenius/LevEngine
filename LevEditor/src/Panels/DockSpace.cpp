#include "pch.h"
#include "DockSpace.h"

#include "Toolbar.h"
#include "Essentials/MenuBar.h"
#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    void DockSpace::Render() const
    {
        LEV_PROFILE_FUNCTION();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y));
        ImGui::SetNextWindowViewport(viewport->ID);

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        constexpr ImGuiWindowFlags windowFlags = 0
                | ImGuiWindowFlags_NoDocking
                | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        GUI::ScopedVariable windowPadding(ImGuiStyleVar_WindowPadding, Vector2::Zero);
        GUI::ScopedVariable windowRounding(ImGuiStyleVar_WindowRounding, 0.0f);
        GUI::ScopedVariable windowBorderSize(ImGuiStyleVar_WindowBorderSize, 0);

        ImGui::Begin("Master DockSpace", nullptr, windowFlags);
        const ImGuiID dockMain = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockMain);

        ImGui::End();
    }
}
