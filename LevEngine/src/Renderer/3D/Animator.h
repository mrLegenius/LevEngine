#pragma once
#include <Math/Matrix.h>

#include "AnimationConstants.h"
#include "DataTypes/Array.h"

namespace LevEngine
{
    class Animation;
    class NodeData;

    class Animator
    {
    public:
        Animator();

        void UpdateAnimation(float deltaTime);
        void PlayAnimation(Ref<Animation> Animation);
        Array<Matrix, AnimationConstants::MaxBoneCount> GetFinalBoneMatrices() const;

    private:
        void CalculateBoneTransform(const NodeData& node, Matrix parentTransform);

        Array<Matrix, AnimationConstants::MaxBoneCount> m_FinalBoneMatrices;
        Ref<Animation> m_CurrentAnimation;
        float m_CurrentTime{};
        float m_DeltaTime{};
    };
}
