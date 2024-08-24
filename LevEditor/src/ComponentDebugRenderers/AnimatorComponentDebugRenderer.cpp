#include "pch.h"
#include "ComponentDebugRenderer.h"

namespace LevEngine::Editor
{
    class AnimatorComponentDebugRenderer final
           : public ComponentDebugRenderer<AnimatorComponent, AnimatorComponentDebugRenderer>
    {
    protected:
        void DrawContent(AnimatorComponent& component, const Entity entity) override
        {
            const auto& transform = entity.GetComponent<Transform>();
            const auto& animatorComponent = entity.GetComponent<AnimatorComponent>();
            
            animatorComponent.DrawDebugPose(transform);
            //animatorComponent.DrawDebugSkeleton(transform);
        }
    };
}
