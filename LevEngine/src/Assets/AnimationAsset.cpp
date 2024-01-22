#include "levpch.h"
#include "AnimationAsset.h"

#include "SkeletonAsset.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	AnimationAsset::AnimationAsset(const Path& path, const UUID uuid) : Asset(path, uuid) {}

	void AnimationAsset::Init(const Ref<Animation>& animation, const Ref<SkeletonAsset>& skeletonAsset)
	{
		SetAnimation(animation);
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

	void AnimationAsset::SetSkeletonAsset(const Ref<SkeletonAsset>& skeletonAsset)
	{
		m_SkeletonAsset = skeletonAsset;
	}

	void AnimationAsset::SerializeData(YAML::Emitter& out)
	{
		SerializeAsset(out, c_SkeletonAssetKey, m_SkeletonAsset);
	}
	void AnimationAsset::DeserializeData(const YAML::Node& node)
	{
		if (const auto skeletonAssetNode = node[c_SkeletonAssetKey])
		{
			SetSkeletonAsset(DeserializeAsset<SkeletonAsset>(skeletonAssetNode));	
		}
	}

	void AnimationAsset::SerializeToBinaryLibrary()
	{
		if (m_Animation == nullptr) return;
        
		m_Animation->Serialize(m_LibraryPath);
	}

	void AnimationAsset::DeserializeFromBinaryLibrary()
	{
		if (m_Animation != nullptr) return;

		m_Animation = CreateRef<Animation>();
		m_Animation->Deserialize(m_LibraryPath);
	}
}
