#include "levpch.h"
#include "SceneManager.h"

#include "Scene.h"
#include "Audio/Audio.h"
#include "Kernel/Application.h"
#include "Serializers/SceneSerializer.h"

namespace LevEngine
{
    events::IEvent<Ref<Scene>>& SceneManager::SceneLoaded = m_OnSceneLoaded;

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
            m_OnSceneLoaded(m_ActiveScene);
            if (App::Get().IsPlaying)
                m_ActiveScene->OnInit();

            return true;
        }

        // Invalidate scene reference if failed to deserialize the scene
        m_ActiveScene = nullptr;
        return false;
    }

    const Ref<Scene>& SceneManager::LoadEmptyScene()
    {
        m_ActiveScenePath = Path();
        m_ActiveScene = CreateRef<Scene>();

        m_OnSceneLoaded(m_ActiveScene);

        return m_ActiveScene;
    }
    void SceneManager::Shutdown()
    {
        m_ActiveScene = nullptr;
    }
}

