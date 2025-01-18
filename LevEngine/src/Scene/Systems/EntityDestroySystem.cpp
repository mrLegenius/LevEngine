#include "levpch.h"
#include "EntityDestroySystem.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

namespace LevEngine
{
    void EntityDestroySystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto& scene = SceneManager::GetActiveScene();
        scene->DestroyAllMarkedEntities();
    }
}