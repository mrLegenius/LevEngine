#pragma once
#include "DataTypes/Vector.h"
#include "EASTL/array.h"
#include "Math/Matrix.h"
#include "Renderer/3D/AnimationConstants.h"
#include "Scene/Components/TypeParseTraits.h"
#include "Scene/Components/Transform/Transform.h"

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
        void ResetInit();

        const Ref<AnimationAsset>& GetAnimationClipConst() const;
        Ref<AnimationAsset>& GetAnimationClip();
        void SetAnimationClip(const Ref<AnimationAsset>& animationClip);

        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);

        void PlayAnimation() const;
        void UpdateAnimation(float deltaTime) const;

        [[nodiscard]] eastl::array<Matrix, AnimationConstants::MaxBoneCount> GetFinalBoneMatrices() const;
        void DrawDebugPose(const Vector3& meshPosition);
        void DrawDebugSkeleton(const Vector3& meshPosition);

    private:        
        Ref<Animator> m_Animator;
        bool m_PlayOnInit{};
        bool m_IsInited{};
    };
}
