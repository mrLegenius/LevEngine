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

			UpdateBoneModelToLocalTransforms(node);
			
			CalculateFinalBoneTransforms(node);
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

	void Animator::PauseAnimation()
	{
		m_IsPlaying = false;
	}

	void Animator::StopAnimation()
	{
		m_CurrentTime = 0.0f;
		m_IsPlaying = false;
	}

	Array<Matrix, AnimationConstants::MaxBoneCount> Animator::GetFinalBoneMatrices() const
	{
		return m_FinalBoneMatrices;
	}

	void Animator::DrawDebugPose(const Transform& rootTransform)
	{
		if (m_CurrentAnimation == nullptr) return;

		const NodeData* node = m_CurrentAnimation->GetRootNode();
		
		DrawDebugPose(node, rootTransform, rootTransform.GetWorldPosition());
	}

	void Animator::DrawDebugPose(const NodeData* node, const Transform& rootTransform, Vector3 prevPosition)
	{
		const String& nodeName = node->name;

		Matrix globalTransformation = node->boneCurrentTransform;
		const NodeData* parent = node->parent;
		if (parent != nullptr) {
			globalTransformation = globalTransformation * parent->boneModelToLocalTransform;
		}
		
		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		Vector3 nextBonePos;
		
		if (boneInfoIt != boneInfoMap.end())
		{
			const Matrix rootMatrix = rootTransform.GetModel();
			nextBonePos = Vector3::Transform(Vector3::Zero, globalTransformation * rootMatrix);
			const Color color = Color(0.0f, 0.0f, 1.0f);
			DebugRender::DrawLine(prevPosition, nextBonePos, color);
			DebugRender::DrawCube(nextBonePos, Vector3::One * 0.05f, color);
		}
		else
		{
			nextBonePos = prevPosition;
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			DrawDebugPose(node->children[i], rootTransform, nextBonePos);
		}
	}

	void Animator::DrawDebugSkeleton(const Transform& rootTransform)
	{
		if (m_CurrentAnimation == nullptr) return;
		
		DrawDebugSkeleton(m_CurrentAnimation->GetRootNode(), rootTransform, rootTransform.GetWorldPosition());
	}

	void Animator::DrawDebugSkeleton(const NodeData* node, const Transform& rootTransform, Vector3 prevPosition)
	{
		const String& nodeName = node->name;
		
		Matrix globalTransformation = node->boneBindPoseTransform;
		const NodeData* parent = node->parent;
		while (parent != nullptr) {
			globalTransformation = parent->boneBindPoseTransform * globalTransformation;
			parent = parent->parent;
		}

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		
		Vector3 nextBonePos;
		const Color color = Color(1.0f, 1.0f, 0.0f);

		if (boneInfoIt != boneInfoMap.end())
		{
			Matrix rootMatrix = rootTransform.GetModel();
			const Matrix finalBoneTransform = (globalTransformation).Transpose();
			nextBonePos = Vector3::Transform(Vector3::Zero, finalBoneTransform * rootMatrix);
			DebugRender::DrawLine(prevPosition, nextBonePos, color);
			DebugRender::DrawCube(nextBonePos, Vector3::One * 0.05f, color);
		}
		else
		{
			nextBonePos = prevPosition;
		}
		
		for (size_t i = 0; i < node->children.size(); i++)
		{
			DrawDebugSkeleton(node->children[i], rootTransform, nextBonePos);
		}
	}

	void Animator::UpdateBoneModelToLocalTransforms(NodeData* node)
	{
		const String& nodeName = node->name;

		if (Bone* Bone = m_CurrentAnimation->FindBone(nodeName))
		{
			Bone->Update(m_CurrentTime);
			node->boneCurrentTransform = Bone->GetLocalTransform();
		}

		for (auto child : node->children)
			UpdateBoneModelToLocalTransforms(child);
	}

	void Animator::CalculateFinalBoneTransforms(NodeData* node)
	{
		const String& nodeName = node->name;
		
		node->boneModelToLocalTransform = node->boneCurrentTransform;
		const NodeData* parent = node->parent;
		if (parent != nullptr) {
			node->boneModelToLocalTransform = node->boneModelToLocalTransform * parent->boneModelToLocalTransform;
		}

		auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		if (boneInfoIt != boneInfoMap.end())
		{
			const int index = boneInfoIt->second.id;
			const Matrix& offset = boneInfoIt->second.offset;

			const Matrix finalBoneTransform = (offset.Transpose() * node->boneModelToLocalTransform);
			m_FinalBoneMatrices[index] = finalBoneTransform;
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			CalculateFinalBoneTransforms(node->children[i]);
		}
	}
}
