#include "levpch.h"
#include "AnimatorUpdateSystem.h"

#include "Scene/Components/Animation/AnimatorComponent.h"

namespace LevEngine
{
    void AnimatorUpdateSystem::Update(float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<AnimatorComponent>();

        for (const auto entity : view)
        {
            auto animatorComponent = view.get<AnimatorComponent>(entity);
            animatorComponent.UpdateAnimation(deltaTime);
        }
    }
}
