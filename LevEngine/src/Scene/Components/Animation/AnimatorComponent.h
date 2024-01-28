#pragma once
#include "Math/Matrix.h"
#include "Renderer/3D/AnimationConstants.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Animation;
    class Animator;
    class AnimationAsset;
    struct Transform;
    class Entity;

    REGISTER_PARSE_TYPE(AnimatorComponent);
    
    struct AnimatorComponent
    {
        static void OnConstruct(Entity entity);
        
        AnimatorComponent();

        const Ref<AnimationAsset>& GetAnimationClipConst() const;
        [[nodiscard]] Ref<AnimationAsset>& GetAnimationClip() const;
        void SetAnimationClip(const Ref<AnimationAsset>& animationClip);

        [[nodiscard]] bool GetPlayOnInit() const;
        void SetPlayOnInit(bool playOnInit);

        void PlayAnimation() const;
        void UpdateAnimation(float deltaTime) const;

        [[nodiscard]] Array<Matrix, AnimationConstants::MaxBoneCount> GetFinalBoneMatrices() const;
        void DrawDebugPose(const Transform& rootTransform) const;
        void DrawDebugSkeleton(const Transform& rootTransform) const;

    private:        
        Ref<Animator> m_Animator;
        bool m_PlayOnInit{};
    };
}
