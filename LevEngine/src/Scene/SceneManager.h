#pragma once

namespace LevEngine
{
    class Scene;

    class SceneManager
    {
    public:

        static void SaveScene(const String& path);
        static void RequestSceneLoad(const Path& path);
        static void TryLoadRequestedScene();
        static bool LoadScene(const Path& path);

        [[nodiscard]] static const Ref<Scene>& GetActiveScene()
        {
            return m_ActiveScene ? m_ActiveScene : LoadEmptyScene();
        }

        [[nodiscard]] static Path GetActiveScenePath()
        {
            return m_ActiveScene ? m_ActiveScenePath : Path();
        }
        
        static const Ref<Scene>& LoadEmptyScene();

        static void Shutdown();

        static events::IEvent<Ref<Scene>>& SceneLoaded;

    private:
        static inline events::TEvent<Ref<Scene>> m_OnSceneLoaded;

        static inline Ref<Scene> m_ActiveScene;
        static inline Path m_ActiveScenePath;

        static inline Path m_RequestedScene;
    };
}
