#pragma once
#include "Asset.h"

namespace LevEngine
{
	class MeshAsset;
	class Animation;

	class AnimationAsset final : public Asset
	{
	public:
		explicit AnimationAsset(const Path& path, const UUID uuid);
		void Init(const Ref<Animation>& animation, int animationIdx, const Ref<MeshAsset>& ownerMesh);

		[[nodiscard]] const Ref<Animation>& GetAnimation() const { return m_Animation; }
		[[nodiscard]] double GetDuration() const;
		[[nodiscard]] const Ref<MeshAsset>& GetOwnerMesh() const;

	protected:
		void SerializeData(YAML::Emitter& out) override;
		void DeserializeData(const YAML::Node& node) override;

	private:
		void SetAnimation(const Ref<Animation>& animation);
		void SetAnimationIdx(int animationIdx);
		void SetOwnerMesh(const Ref<MeshAsset>& ownerMesh);
		
		const char* c_AnimationIndexKey = "AnimationIndexInModelFile";
		const char* c_OwnerMeshKey = "OwnerMesh";
		
		Ref<Animation> m_Animation;
		int m_AnimationIdx;
		Ref<MeshAsset> m_OwnerMesh;
	};
}


