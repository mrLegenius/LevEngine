#include "levpch.h"
#include "CharacterControllerInitSystem.h"

#include "Physics/Components/CharacterController.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void CharacterControllerInitSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<Transform, CharacterController>();

        for (const auto entity : view)
        {
            auto [transform, characterController] = view.get<Transform, CharacterController>(entity);

            if (!characterController.IsInitialized())
            {
                const auto gameObject = Entity(entt::handle(registry, entity));
                characterController.Initialize(gameObject);
            }
        }
    }
}