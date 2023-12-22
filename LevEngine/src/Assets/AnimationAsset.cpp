#include "levpch.h"
#include "AnimationAsset.h"

#include "MeshAsset.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
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

	const Ref<MeshAsset>& AnimationAsset::GetOwnerMesh() const
	{
		return m_OwnerMesh;
	}

	void AnimationAsset::SetOwnerMesh(const Ref<MeshAsset>& ownerMesh)
	{
		m_OwnerMesh = ownerMesh;
	}

	void AnimationAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << c_AnimationIndexKey << YAML::Value << m_AnimationIdx;
		SerializeAsset(out, c_OwnerMeshKey, m_OwnerMesh);
	}
	void AnimationAsset::DeserializeData(YAML::Node& node)
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

		m_OwnerMesh = DeserializeAsset<MeshAsset>(node[c_OwnerMeshKey]);
		if (m_OwnerMesh == nullptr)
		{
			Log::CoreWarning("Failed to deserialize owner mesh of animation in {0}",
				m_Path.string());
		}

		if (m_AnimationIdx == -1 || m_OwnerMesh == nullptr)
		{
			return;
		}

		try
		{
			SetAnimation(AnimationLoader::LoadAnimation(m_OwnerMesh->GetPath(), m_OwnerMesh->GetMesh(), m_AnimationIdx));
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to load animation in {0} with animation index: {1}. Error: {2}",
				m_Path.string(), m_AnimationIdx, e.what());
		}
	}
}
