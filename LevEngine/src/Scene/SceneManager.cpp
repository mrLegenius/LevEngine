#include "levpch.h"
#include "SceneManager.h"

#include "Serializers/SceneSerializer.h"

namespace LevEngine
{
    void SceneManager::SaveScene(const String& path)
    {
        const SceneSerializer sceneSerializer(m_ActiveScene);
        sceneSerializer.Serialize(path);
    }

    bool SceneManager::LoadScene(const Path& path)
    {
        const Ref<Scene>& newScene = CreateRef<Scene>();
        SceneSerializer sceneSerializer(newScene);

        if (sceneSerializer.Deserialize(path.generic_string().c_str()))
        {
            m_ActiveScene = newScene;
            return true;
        }

        return false;
    }

    const Ref<Scene>& SceneManager::LoadEmptyScene()
    {
        return m_ActiveScene = CreateRef<Scene>();
    }
}

