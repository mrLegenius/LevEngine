#pragma once
#include "Scene.h"
#include "DataTypes/Path.h"

namespace LevEngine
{
    class SceneManager
    {
    public:

        static void SaveScene(const String& path);

        [[nodiscard]] static const Ref<Scene>& GetActiveScene()
        {
            return m_ActiveScene ? m_ActiveScene : LoadEmptyScene();
        }

        static bool LoadScene(const Path& path);

        static const Ref<Scene>& LoadEmptyScene();

    private:
        static inline Ref<Scene> m_ActiveScene;
    };
}
