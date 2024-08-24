#pragma once

namespace LevEngine::Editor
{
    enum class SceneState;
    class MenuBar;
    
    class Toolbar
    {
    public:
        explicit Toolbar(const Ref<MenuBar>& mainMenuBar, const Func<SceneState>& sceneStateGetter, const Action<>& sceneStateButtonClicked);
        void Render();
        [[nodiscard]] float GetHeight() const { return m_Height; }
    private:
        void DrawTools() const;
        void DrawViewportControlButtons() const;
        void DrawSceneStateButton() const;

        Ref<MenuBar> m_MainMenuBar;
        float m_Height;
        Func<SceneState> m_SceneStateGetter;
        Action<> m_SceneStateButtonClicked;
    };
}
