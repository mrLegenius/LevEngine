#pragma once
#include "Asset.h"

namespace LevEngine
{
	class ModelAsset;
	class Animation;

	class AnimationAsset final : public Asset
	{
	public:
		explicit AnimationAsset(const Path& path, UUID uuid);
		void Init(const Ref<Animation>& animation, int animationIdx, const Ref<ModelAsset>& owner);

		[[nodiscard]] const Ref<Animation>& GetAnimation() const { return m_Animation; }
		[[nodiscard]] double GetDuration() const;
		[[nodiscard]] const Ref<ModelAsset>& GetOwnerModel() const;

	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(const YAML::Node& node) override;

	private:
		void SetAnimation(const Ref<Animation>& animation);
		void SetAnimationIdx(int animationIdx);
		void SetOwnerModel(const Ref<ModelAsset>& owner);
		
		const char* c_AnimationIndexKey = "AnimationIndexInModelFile";
		const char* c_OwnerMeshKey = "OwnerMesh";
		
		Ref<Animation> m_Animation;
		int m_AnimationIdx;
		Ref<ModelAsset> m_OwnerModel;
	};
}


