#include "pch.h"

#include "ComponentDrawer.h"
#include "Scene/Components/Animation/AnimatorComponent.h"

namespace LevEngine::Editor
{
    class AnimatorComponentDrawer final : public ComponentDrawer<AnimatorComponent, AnimatorComponentDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Animator"; }

        void DrawContent(AnimatorComponent& component) override
        {
            EditorGUI::DrawAsset("Animation Clip", component.GetAnimationClip());
            EditorGUI::DrawCheckBox("Play On Init", BindGetter(&AnimatorComponent::GetPlayOnInit, &component),
                BindSetter(&AnimatorComponent::SetPlayOnInit, &component));
        }
    };	
}
