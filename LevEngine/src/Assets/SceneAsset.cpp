#include "levpch.h"
#include "SceneAsset.h"

#include "Scene/SceneManager.h"

namespace LevEngine
{
    void SceneAsset::Load() const
    {
        SceneManager::LoadScene(m_Path);
    }
}
