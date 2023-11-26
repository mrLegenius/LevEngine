#pragma once
#include "DataTypes/Vector.h"
#include "EASTL/array.h"
#include "Math/Matrix.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Animation;
    class Animator;
    class AnimationAsset;

    REGISTER_PARSE_TYPE(AnimatorComponent);
    
    struct AnimatorComponent
    {
        AnimatorComponent();

        void Init();
        bool IsInitialized() const;

        const Ref<AnimationAsset>& GetAnimationClipConst() const;
        Ref<AnimationAsset>& GetAnimationClip();
        void SetAnimationClip(const Ref<AnimationAsset>& animationClip);

        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);

        void PlayAnimation() const;
        void UpdateAnimation(float deltaTime) const;

        [[nodiscard]] eastl::array<Matrix, 100> GetFinalBoneMatrices() const;

    private:        
        Ref<Animator> m_Animator;
        Ref<AnimationAsset> m_AnimationClip;
        bool m_PlayOnInit{};
        bool m_IsInited{};
    };
}
