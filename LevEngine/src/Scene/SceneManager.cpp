#include "levpch.h"
#include "SceneManager.h"

#include "Audio/LevFmod.h"
#include "Serializers/SceneSerializer.h"
#include "Kernel/Application.h"

namespace LevEngine
{
    void SceneManager::SaveScene(const String& path)
    {
        m_ActiveScenePath = path.c_str();
        const SceneSerializer sceneSerializer(m_ActiveScene);
        sceneSerializer.Serialize(path.c_str());
    }

    bool SceneManager::LoadScene(const Path& path)
    {
        if (m_ActiveScene != nullptr)
        {
            m_ActiveScene->CleanupScene();
        }

        const Ref<Scene>& newScene = CreateRef<Scene>();
        SceneSerializer sceneSerializer(newScene);

        m_ActiveScene = newScene;
        
        if (sceneSerializer.Deserialize(path.generic_string().c_str()))
        {
            m_ActiveScenePath = path;
            return true;
        }

        // Invalidate scene reference if failed to deserialize the scene
        m_ActiveScene = nullptr;
        return false;
    }

    const Ref<Scene>& SceneManager::LoadEmptyScene()
    {
        m_ActiveScenePath = Path();
        return m_ActiveScene = CreateRef<Scene>();
    }
}

