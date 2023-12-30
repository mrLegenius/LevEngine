#include "levpch.h"
#include "EntityDestroySystem.h"

#include "Physics/Components/Destroyable.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void EntityDestroySystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto destroyableView = registry.view<Transform, Destroyable>();

        for (const auto entity : destroyableView)
        {
            const auto scene = SceneManager::GetActiveScene();

            const auto entityToDestroy = Entity(entt::handle(registry, entity));

            scene->DestroyEntityImmediate(entityToDestroy);
        }
    }
}