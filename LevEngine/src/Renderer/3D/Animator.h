#pragma once
#include <DataTypes/Vector.h>
#include <Math/Matrix.h>

namespace LevEngine
{
    class Animation;
    class NodeData;

    class Animator
    {
    public:
        Animator(const Ref<Animation>& Animation);

        void UpdateAnimation(float deltaTime);

        void PlayAnimation(Ref<Animation> Animation);

        void CalculateBoneTransform(const NodeData& node, Matrix parentTransform);

        Vector<Matrix> GetFinalBoneMatrices();

    private:
        static const int MaxBoneCount = 100;

        Vector<Matrix> m_FinalBoneMatrices;
        Ref<Animation> m_CurrentAnimation;
        float m_CurrentTime;
        float m_DeltaTime;
    };
}
