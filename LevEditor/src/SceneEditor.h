#pragma once

namespace LevEngine::Editor
{
    enum class SceneState;
    class MenuBar;

    class SceneEditor
    {
    public:
        explicit SceneEditor(const Func<SceneState>& sceneStateGetter);

        static void OnDuplicateEntity();
        static void CreateNewScene();
        void OpenScene() const;
        bool OpenScene(const Path& path) const;
        bool SaveScene() const;
        bool SaveSceneAs() const;

        bool OnKeyPressed(const KeyPressedEvent& e) const;

        void AddMainMenuItems(const Ref<MenuBar>& menuBar) const;
    private:
        Func<SceneState> m_SceneStateGetter;
    };
}
