#pragma once
#include "Asset.h"

namespace LevEngine
{
	class MeshAsset;
	class Animation;

	class AnimationAsset final : public Asset
	{
	public:
		explicit AnimationAsset(const Path& path, const UUID uuid) : Asset(path, uuid)
		{
			m_AnimationIdx = -1;
		}

		[[nodiscard]] const Ref<Animation>& GetAnimation() const { return m_Animation; }

		[[nodiscard]] double GetDuration() const;
		
		void SetAnimation(const Ref<Animation>& animation);
		void SetAnimationIdx(int animationIdx);

		[[nodiscard]] const Ref<MeshAsset>& GetOwnerMesh() const;
		void SetOwnerMesh(const Ref<MeshAsset>& ownerMesh);

	protected:
		void SerializeData(YAML::Emitter& out) override;

		void DeserializeData(YAML::Node& node) override;

	private:
		const char* c_AnimationIndexKey = "AnimationIndexInModelFile";
		const char* c_OwnerMeshKey = "OwnerMesh";
		
		Ref<Animation> m_Animation;
		int m_AnimationIdx;
		Ref<MeshAsset> m_OwnerMesh;
	};
}


