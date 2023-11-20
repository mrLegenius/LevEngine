#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class AnimationAsset;

    REGISTER_PARSE_TYPE(AnimatorComponent);
    
    struct AnimatorComponent
    {
        AnimatorComponent() = default;

        const Ref<AnimationAsset>& GetAnimationClipConst() const;
        Ref<AnimationAsset>& GetAnimationClip();
        void SetAnimationClip(const Ref<AnimationAsset>& animationClip);

    private:
        Ref<AnimationAsset> m_AnimationClip;
    };
}
