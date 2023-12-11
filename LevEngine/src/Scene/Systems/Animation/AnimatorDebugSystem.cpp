#include "levpch.h"
#include "AnimatorDebugSystem.h"

#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void AnimatorDebugSystem::Update(float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<AnimatorComponent, Transform>();

        for (const auto entity : view)
        {
            auto [animatorComponent, transform] = view.get<AnimatorComponent, Transform>(entity);
            Vector3 position = transform.GetWorldPosition();
            animatorComponent.DrawDebugPose(transform);
            //animatorComponent.DrawDebugSkeleton(transform);
        }
    }
}
