#include "levpch.h"
#include "Animator.h"
#include "Animation.h"
#include "AnimationConstants.h"

namespace LevEngine
{
	Animator::Animator()
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = nullptr;

		m_FinalBoneMatrices = Array<Matrix, AnimationConstants::MaxBoneCount>();

		for (int i = 0; i < AnimationConstants::MaxBoneCount; i++)
		{
			m_FinalBoneMatrices[i] = Matrix::Identity;
		}
	}

	void Animator::UpdateAnimation(float deltaTime)
	{
		m_DeltaTime = deltaTime;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * deltaTime;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(m_CurrentAnimation->GetRootNode(), Matrix::Identity);
		}
	}

	void Animator::PlayAnimation(Ref<Animation> Animation)
	{
		m_CurrentAnimation = Animation;
		m_CurrentTime = 0.0f;
	}

	Array<Matrix, AnimationConstants::MaxBoneCount> Animator::GetFinalBoneMatrices() const
	{
		return m_FinalBoneMatrices;
	}

	void Animator::CalculateBoneTransform(const NodeData& node, Matrix parentTransform)
	{
		String nodeName = node.name;
		Matrix nodeTransform = node.transformation;

		if (Bone* Bone = m_CurrentAnimation->FindBone(nodeName))
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		Matrix globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		auto boneInfoIt = boneInfoMap.find(nodeName);
		if (boneInfoIt != boneInfoMap.end())
		{
			int index = boneInfoIt->second.id;
			Matrix offset = boneInfoIt->second.offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (unsigned int i = 0; i < node.childrenCount; i++)
		{
			CalculateBoneTransform(node.children[i], globalTransformation);
		}
	}
}
