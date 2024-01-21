#pragma once
#include "Asset.h"

namespace LevEngine
{
	class Skeleton;
	class SkeletonAsset;
}

namespace LevEngine
{
	class MeshAsset;
	class Animation;

	class AnimationAsset final : public Asset
	{
	public:
		explicit AnimationAsset(const Path& path, const UUID uuid);
		void Init(const Ref<Animation>& animation, int animationIdx, const Ref<SkeletonAsset>& skeletonAsset);

		[[nodiscard]] const Ref<Animation>& GetAnimation() const { return m_Animation; }
		[[nodiscard]] double GetDuration() const;
		[[nodiscard]] const Ref<Skeleton>& GetSkeleton() const;

	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(const YAML::Node& node) override;

	private:
		void SetAnimation(const Ref<Animation>& animation);
		void SetAnimationIdx(int animationIdx);
		void SetSkeletonAsset(const Ref<SkeletonAsset>& skeletonAsset);
		
		const char* c_AnimationIndexKey = "AnimationIndexInModelFile";
		const char* c_SkeletonAssetKey = "SkeletonAsset";
		
		Ref<Animation> m_Animation;
		int m_AnimationIdx;
		Ref<SkeletonAsset> m_SkeletonAsset;
	};
}


