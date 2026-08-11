#include "pch.h"
#include "DockSpace.h"

#include "PanelManager.h"
#include "PanelTypes.h"
#include "Toolbar.h"
#include "Essentials/MenuBar.h"
#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    void DockSpace::Render(PanelManager& panels)
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

        //There is no layout when imgui.ini is missing, when the panels were reset to the
        //default set or when the reset was requested explicitly
        const bool panelsWereReset = panels.ConsumeDefaultLayoutRequest();
        if (m_IsDefaultLayoutRequested || panelsWereReset || ImGui::DockBuilderGetNode(dockMain) == nullptr)
        {
            m_IsDefaultLayoutRequested = false;
            BuildDefaultLayout(dockMain, panels);
        }

        ImGui::DockSpace(dockMain);

        ImGui::End();
    }

    void DockSpace::BuildDefaultLayout(const ImGuiID dockSpaceId, const PanelManager& panels)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::DockBuilderRemoveNode(dockSpaceId);
        ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockSpaceId, viewport->WorkSize);

        ImGuiID center = dockSpaceId;
        ImGuiID bottom = ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.37f, nullptr, &center);
        //Properties are on the far right, hierarchy is the narrow column next to them
        const ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.29f, nullptr, &center);
        const ImGuiID centerRight = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.16f, nullptr, &center);
        const ImGuiID bottomRight = ImGui::DockBuilderSplitNode(bottom, ImGuiDir_Right, 0.54f, nullptr, &bottom);

        //Only the panels that are opened right now can be docked
        const auto dock = [&panels](const String& typeName, const ImGuiID node)
        {
            const auto windowName = panels.GetWindowName(typeName);
            if (windowName.empty()) return;

            ImGui::DockBuilderDockWindow(windowName.c_str(), node);
        };

        dock(PanelTypes::Viewport, center);
        dock(PanelTypes::Game, center);
        dock(PanelTypes::Hierarchy, centerRight);
        dock(PanelTypes::Scripts, centerRight);
        dock(PanelTypes::Properties, right);
        dock(PanelTypes::Settings, right);
        dock(PanelTypes::Statistics, right);
        dock(PanelTypes::Console, bottom);
        dock(PanelTypes::AssetBrowser, bottomRight);

        ImGui::DockBuilderFinish(dockSpaceId);
    }
}
