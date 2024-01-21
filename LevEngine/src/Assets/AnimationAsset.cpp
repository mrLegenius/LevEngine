#include "levpch.h"
#include "AnimationAsset.h"

#include "SkeletonAsset.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	AnimationAsset::AnimationAsset(const Path& path, const UUID uuid) : Asset(path, uuid), m_AnimationIdx(-1) {}

	void AnimationAsset::Init(const Ref<Animation>& animation, int animationIdx, const Ref<SkeletonAsset>& skeletonAsset)
	{
		SetAnimation(animation);
		SetAnimationIdx(animationIdx);
		SetSkeletonAsset(skeletonAsset);
	}

	double AnimationAsset::GetDuration() const
	{
		return m_Animation->GetDuration();
	}

	const Ref<Skeleton>& AnimationAsset::GetSkeleton() const
	{
		return m_SkeletonAsset->GetSkeleton();
	}

	void AnimationAsset::SetAnimation(const Ref<Animation>& animation)
	{
		m_Animation = animation;
	}

	void AnimationAsset::SetAnimationIdx(int animationIdx)
	{
		m_AnimationIdx = animationIdx;
	}

	void AnimationAsset::SetSkeletonAsset(const Ref<SkeletonAsset>& skeletonAsset)
	{
		m_SkeletonAsset = skeletonAsset;
	}

	void AnimationAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << c_AnimationIndexKey << YAML::Value << m_AnimationIdx;
		SerializeAsset(out, c_SkeletonAssetKey, m_SkeletonAsset);
	}
	void AnimationAsset::DeserializeData(const YAML::Node& node)
	{
		if (const auto animationIndexNode = node[c_AnimationIndexKey])
		{
			SetAnimationIdx(animationIndexNode.as<int>());
		}
		else
		{
			Log::CoreWarning("Failed to deserialize animation in {0}, got invalid animation index: {1}",
				m_Path.string(), m_AnimationIdx);
		}

		if (const auto skeletonAssetNode = node[c_SkeletonAssetKey])
		{
			SetSkeletonAsset(DeserializeAsset<SkeletonAsset>(skeletonAssetNode));	
		}
	}
}
