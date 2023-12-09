#include "levpch.h"
#include "Animator.h"
#include "Animation.h"
#include "AnimationConstants.h"
#include "Renderer/DebugRender/DebugRender.h"

namespace LevEngine
{
	Animator::Animator()
	{
		m_CurrentTime = 0.0;
		m_IsPlaying = false;
		m_CurrentAnimation = nullptr;
		m_CurrentAnimationAsset = nullptr;

		m_FinalBoneMatrices = Array<Matrix, AnimationConstants::MaxBoneCount>();

		for (int i = 0; i < AnimationConstants::MaxBoneCount; i++)
		{
			m_FinalBoneMatrices[i] = Matrix::Identity;
		}
	}

	void Animator::UpdateAnimation(float deltaTime)
	{
		if (!m_IsPlaying) return;
		
		m_DeltaTime = deltaTime;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += static_cast<float>(m_CurrentAnimation->GetTicksPerSecond()) * deltaTime;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

			NodeData* node = m_CurrentAnimation->GetRootNode();
			UpdateLocalBoneTransforms(node);
			
			CalculateBoneTransform(node);
		}
	}

	Ref<AnimationAsset>& Animator::GetAnimationClip()
	{
		return m_CurrentAnimationAsset;
	}

	void Animator::SetAnimationClip(const Ref<AnimationAsset>& animationAsset)
	{
		m_CurrentAnimationAsset = animationAsset;
		m_CurrentAnimation = animationAsset->GetAnimation();
	}

	void Animator::PlayAnimation()
	{
		m_CurrentTime = 0.0f;
		m_IsPlaying = true;
	}

	Array<Matrix, AnimationConstants::MaxBoneCount> Animator::GetFinalBoneMatrices() const
	{
		return m_FinalBoneMatrices;
	}

	void Animator::DrawDebugPose(const Vector3& meshPosition)
	{
		if (m_CurrentAnimation == nullptr) return;

		UpdateLocalBoneTransforms(m_CurrentAnimation->GetRootNode());
		
		DrawDebugPose(m_CurrentAnimation->GetRootNode(), meshPosition, meshPosition);
	}

	void Animator::DrawDebugPose(const NodeData* node, const Vector3& meshPosition, Vector3 prevPosition)
	{
		const String& nodeName = node->name;

		Matrix globalTransformation = node->localTransform;
		const NodeData* parent = node->parent;
		while (parent != nullptr) {
			globalTransformation *= parent->localTransform;
			parent = parent->parent;
		}
		
		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		Vector3 nextBonePos;
		
		if (boneInfoIt != boneInfoMap.end())
		{
			const Matrix& rootInverseTransform = m_CurrentAnimation->GetRootInverseTransform();
			const Matrix finalBoneTransform = globalTransformation * rootInverseTransform;
			nextBonePos = Vector3::Transform(meshPosition, finalBoneTransform);
			const Color color = Color(0.0f, 0.0f, 1.0f);
			DebugRender::DrawLine(prevPosition, nextBonePos, color);
		}
		else
		{
			nextBonePos = prevPosition;
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			DrawDebugPose(node->children[i], meshPosition, nextBonePos);
		}
	}

	void Animator::DrawDebugSkeleton(const Vector3& meshPosition)
	{
		if (m_CurrentAnimation == nullptr) return;
		
		DrawDebugSkeleton(m_CurrentAnimation->GetRootNode(), meshPosition, meshPosition);
	}

	void Animator::DrawDebugSkeleton(const NodeData* node, const Vector3& meshPosition, Vector3 prevPosition)
	{
		const String& nodeName = node->name;
		
		Matrix globalTransformation = node->originalTransform;
		const NodeData* parent = node->parent;
		while (parent != nullptr) {
			globalTransformation *= parent->originalTransform;
			parent = parent->parent;
		}

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		
		Vector3 nextBonePos;

		if (boneInfoIt != boneInfoMap.end())
		{
			const Matrix& rootInverseTransform = m_CurrentAnimation->GetRootInverseTransform();
			const Matrix finalBoneTransform = globalTransformation * rootInverseTransform;
			nextBonePos = Vector3::Transform(meshPosition, finalBoneTransform);
			const Color color = Color(1.0f, 1.0f, 0.0f);
			DebugRender::DrawLine(prevPosition, nextBonePos, color);
		}
		else
		{
			nextBonePos = prevPosition;
		}
		
		for (size_t i = 0; i < node->children.size(); i++)
		{
			DrawDebugSkeleton(node->children[i], meshPosition, nextBonePos);
		}
	}

	void Animator::UpdateLocalBoneTransforms(NodeData* node)
	{
		const String& nodeName = node->name;

		if (Bone* Bone = m_CurrentAnimation->FindBone(nodeName))
		{
			Bone->Update(m_CurrentTime);
			node->localTransform = Bone->GetLocalTransform();
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			UpdateLocalBoneTransforms(node->children[i]);
		}
	}

	void Animator::CalculateBoneTransform(NodeData* node)
	{
		const String& nodeName = node->name;
		
		node->globalTransform = node->localTransform;
		const NodeData* parent = node->parent;
		while (parent != nullptr) {
			node->globalTransform *= parent->localTransform;
			parent = parent->parent;
		}

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		if (boneInfoIt != boneInfoMap.end())
		{
			const int index = boneInfoIt->second.id;
			const Matrix& offset = boneInfoIt->second.offset;
			const Matrix& rootInverseTransform = m_CurrentAnimation->GetRootInverseTransform();
			const Matrix finalBoneTransform = offset * node->globalTransform * rootInverseTransform;
			m_FinalBoneMatrices[index] = finalBoneTransform;
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			CalculateBoneTransform(node->children[i]);
		}
	}
}
