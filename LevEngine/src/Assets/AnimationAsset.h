#pragma once
#include "Asset.h"

namespace LevEngine
{
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

	protected:
		void SerializeData(YAML::Emitter& out) override;

		void DeserializeData(YAML::Node& node) override;

	private:
		const char* c_AnimationIndexKey = "AnimationIndexInModelFile";
		
		Ref<Animation> m_Animation;
		int m_AnimationIdx;
	};
}


