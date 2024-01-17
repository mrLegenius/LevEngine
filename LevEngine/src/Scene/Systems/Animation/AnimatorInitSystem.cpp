#include "levpch.h"
#include "AnimatorInitSystem.h"

#include "Scene/Components/Animation/AnimatorComponent.h"

namespace LevEngine
{
    void AnimatorInitSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<AnimatorComponent>();

        for (const auto entity : view)
        {
            AnimatorComponent& animatorComponent = view.get<AnimatorComponent>(entity);

            if (!animatorComponent.IsInitialized())
            {
                animatorComponent.Init();
            }
        }
    }
}