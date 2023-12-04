#pragma once
#include <Math/Matrix.h>

#include "AnimationConstants.h"
#include "Assets/AnimationAsset.h"
#include "DataTypes/Array.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    class Animation;
    struct NodeData;

    class Animator
    {
    public:
        Animator();

        void UpdateAnimation(float deltaTime);
        [[nodiscard]] Ref<AnimationAsset>& GetAnimationClip();
        void SetAnimationClip(const Ref<AnimationAsset>& animationAsset);
        void PlayAnimation();
        [[nodiscard]] Array<Matrix, AnimationConstants::MaxBoneCount> GetFinalBoneMatrices() const;
        void DrawDebugPose(const Vector3& meshPosition);
        void DrawDebugPose(const NodeData* node, const Vector3& meshPosition, Vector3 prevPosition);
        void DrawDebugSkeleton(const Vector3& meshPosition);
        void DrawDebugSkeleton(const NodeData* node, const Vector3& meshPosition, Vector3 prevPosition);

    private:
        void UpdateLocalBoneTransforms(NodeData* node);
        void CalculateBoneTransform(NodeData* node);

        Array<Matrix, AnimationConstants::MaxBoneCount> m_FinalBoneMatrices;
        Ref<AnimationAsset> m_CurrentAnimationAsset;
        Ref<Animation> m_CurrentAnimation;
        float m_CurrentTime{};
        float m_DeltaTime{};
        bool m_IsPlaying{};
    };
}
