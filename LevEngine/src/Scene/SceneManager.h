#pragma once
#include "Scene.h"
#include "DataTypes/Path.h"

namespace LevEngine
{
    class SceneManager
    {
    public:

        static void SaveScene(const String& path);
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

    private:
        static inline Ref<Scene> m_ActiveScene;
        static inline Path m_ActiveScenePath;
    };
}
