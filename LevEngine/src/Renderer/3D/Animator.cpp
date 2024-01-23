#include "levpch.h"
#include "Animator.h"
#include "Animation.h"
#include "AnimationConstants.h"
#include "Renderer/DebugRender/DebugRender.h"
#include "Skeleton.h"
#include "SkeletonNodeData.h"
#include "BoneInfo.h"
#include <chrono>

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
		LEV_PROFILE_FUNCTION();
		
		if (!m_IsPlaying) return;
		
		m_DeltaTime = deltaTime;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += static_cast<float>(m_CurrentAnimation->GetTicksPerSecond()) * deltaTime;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

			const Ref<SkeletonNodeData> node = m_CurrentSkeleton->GetRootNode();
			
			UpdateBoneModelToLocalTransforms(node);

			//auto t1 = high_resolution_clock::now();
			CalculateFinalBoneTransforms(node, Matrix::Identity);
			//auto t2 = high_resolution_clock::now();
			//duration<double, std::milli> ms_double = t2 - t1;

			//Log::CoreWarning("CalculateFinalBoneTransforms time: {0} ms", ms_double.count());
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
		m_CurrentSkeleton = animationAsset->GetSkeleton();
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

		const Ref<SkeletonNodeData> node = m_CurrentSkeleton->GetRootNode();

		DrawDebugPose(node, rootTransform, rootTransform.GetWorldPosition(), Matrix::Identity);
	}

	void Animator::DrawDebugPose(const Ref<SkeletonNodeData>& node, const Transform& rootTransform, Vector3 prevPosition,
		Matrix parentModelToLocalTransform)
	{
		const String& nodeName = node->name;

		if (const Bone* bone = m_CurrentAnimation->FindBone(nodeName))
		{
			parentModelToLocalTransform = bone->GetLocalTransform() * parentModelToLocalTransform;
		}
		else
		{
			parentModelToLocalTransform = node->boneBindPoseTransform * parentModelToLocalTransform;
		}
		
		auto boneInfoMap = m_CurrentSkeleton->GetBoneInfoMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		Vector3 nextBonePos;
		
		if (boneInfoIt != boneInfoMap.end())
		{
			const Matrix rootMatrix = rootTransform.GetModel();
			nextBonePos = Vector3::Transform(Vector3::Zero, parentModelToLocalTransform * rootMatrix);
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
			DrawDebugPose(node->children[i], rootTransform, nextBonePos, parentModelToLocalTransform);
		}
	}

	void Animator::DrawDebugSkeleton(const Transform& rootTransform)
	{
		if (m_CurrentAnimation == nullptr) return;
		
		DrawDebugSkeleton(m_CurrentSkeleton->GetRootNode(), rootTransform, rootTransform.GetWorldPosition());
	}

	void Animator::DrawDebugSkeleton(const Ref<SkeletonNodeData>& node, const Transform& rootTransform, Vector3 prevPosition)
	{
		const String& nodeName = node->name;
		
		Matrix globalTransformation = node->boneBindPoseTransform;
		Ref<SkeletonNodeData> parent = node->parent;
		while (parent != nullptr) {
			globalTransformation = parent->boneBindPoseTransform * globalTransformation;
			parent = parent->parent;
		}

		auto boneInfoMap = m_CurrentSkeleton->GetBoneInfoMap();
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

	void Animator::UpdateBoneModelToLocalTransforms(const Ref<SkeletonNodeData>& node)
	{
		LEV_PROFILE_FUNCTION();
		
		const String& nodeName = node->name;

		if (Bone* Bone = m_CurrentAnimation->FindBone(nodeName))
		{
			Bone->Update(m_CurrentTime);
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			UpdateBoneModelToLocalTransforms(node->children[i]);
		}
	}

	void Animator::CalculateFinalBoneTransforms(const Ref<SkeletonNodeData>& node, Matrix parentModelToLocalTransform)
	{
		LEV_PROFILE_FUNCTION();

		const String& nodeName = node->name;

		if (const Bone* bone = m_CurrentAnimation->FindBone(nodeName))
		{
			parentModelToLocalTransform = bone->GetLocalTransform() * parentModelToLocalTransform;
		}
		else
		{
			parentModelToLocalTransform = node->boneBindPoseTransform * parentModelToLocalTransform;
		}

		auto boneInfoMap = m_CurrentSkeleton->GetBoneInfoMap();
		const auto boneInfoIt = boneInfoMap.find(nodeName);
		if (boneInfoIt != boneInfoMap.end())
		{
			LEV_PROFILE_SCOPE("Calculate final bone transform");
			
			const size_t index = boneInfoIt->second.id;
			const Matrix& offset = boneInfoIt->second.offset;

			const Matrix finalBoneTransform = (offset * parentModelToLocalTransform);
			m_FinalBoneMatrices[index] = finalBoneTransform;
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			CalculateFinalBoneTransforms(node->children[i], parentModelToLocalTransform);
		}
	}
}
