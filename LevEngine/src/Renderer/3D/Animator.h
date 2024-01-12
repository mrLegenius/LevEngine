#pragma once
#include <Math/Matrix.h>

#include "AnimationConstants.h"
#include "Assets/AnimationAsset.h"
#include "DataTypes/Array.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    class Animation;
    struct SkeletonNodeData;

    class Animator
    {
    public:
        Animator();

        void UpdateAnimation(float deltaTime);
        [[nodiscard]] Ref<AnimationAsset>& GetAnimationClip();
        void SetAnimationClip(const Ref<AnimationAsset>& animationAsset);
        void PlayAnimation();
        void PauseAnimation();
        void StopAnimation();
        [[nodiscard]] Array<Matrix, AnimationConstants::MaxBoneCount> GetFinalBoneMatrices() const;
        void DrawDebugPose(const Transform& rootTransform);
        void DrawDebugPose(const SkeletonNodeData* node, const Transform& rootTransform, Vector3 prevPosition,
            Matrix& parentModelToLocalTransform);
        void DrawDebugSkeleton(const Transform& rootTransform);
        void DrawDebugSkeleton(const SkeletonNodeData* node, const Transform& rootTransform, Vector3 prevPosition);

    private:
        void UpdateBoneModelToLocalTransforms(SkeletonNodeData* node);
        void CalculateFinalBoneTransforms(SkeletonNodeData* node, Matrix& parentModelToLocalTransform);

        Array<Matrix, AnimationConstants::MaxBoneCount> m_FinalBoneMatrices;
        Ref<AnimationAsset> m_CurrentAnimationAsset;
        Ref<Animation> m_CurrentAnimation;
        double m_CurrentTime{};
        float m_DeltaTime{};
        bool m_IsPlaying{};
    };
}
