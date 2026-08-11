#pragma once

namespace LevEngine::Editor
{
    class PanelManager;

    class DockSpace
    {
    public:
        void Render(PanelManager& panels);
        //Rebuilds the default layout on the next frame
        void RequestDefaultLayout() { m_IsDefaultLayoutRequested = true; }

    private:
        static void BuildDefaultLayout(ImGuiID dockSpaceId, const PanelManager& panels);

        bool m_IsDefaultLayoutRequested = false;
    };
}
