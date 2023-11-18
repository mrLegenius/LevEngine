#include "levpch.h"
#include "AnimationAsset.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Renderer/3D/MeshLoading/MeshLoader.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	double AnimationAsset::GetDuration() const
	{
		return m_Animation->GetDuration();
	}

	void AnimationAsset::SetAnimation(const Ref<Animation>& animation)
	{
		m_Animation = animation;
	}

	void AnimationAsset::SetAnimationIdx(int animationIdx)
	{
		m_AnimationIdx = animationIdx;
	}

	void AnimationAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << c_AnimationIndexKey << YAML::Value << m_AnimationIdx;
	}
	void AnimationAsset::DeserializeData(YAML::Node& node)
	{
		if (const auto animationIndexNode = node[c_AnimationIndexKey])
		{
			SetAnimationIdx(animationIndexNode.as<int>());
		}

		if (m_AnimationIdx == -1)
		{
			Log::CoreWarning("Failed to deserialize animation in {0}, got invalid animation index: {1}",
				m_Path.string(), m_AnimationIdx);
			return;
		}

		try
		{
			Ref<Mesh> mesh = MeshLoader::LoadMesh(m_Path);
			SetAnimation(AnimationLoader::LoadAnimation(m_Path, mesh, m_AnimationIdx));
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to load animation in {0} with animation index: {1}. Error: {2}",
				m_Path.string(), m_AnimationIdx, e.what());
		}
	}
}
