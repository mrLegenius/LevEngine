#pragma once

namespace LevEngine::Editor
{
    class Toolbar;
    class MenuBar;

    class DockSpace
    {
    public:
        explicit DockSpace(const Ref<Toolbar>& toolbar, const Ref<MenuBar>& menuBar);
        
        void Render() const;
    private:
        Ref<Toolbar> m_MainToolBar;
        Ref<MenuBar> m_MainMenuBar;
    };
}
