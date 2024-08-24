#include "levpch.h"
#include "AnimationAsset.h"

#include "MeshAsset.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	AnimationAsset::AnimationAsset(const Path& path, const UUID uuid) : Asset(path, uuid), m_AnimationIdx(-1) {}

	void AnimationAsset::Init(const Ref<Animation>& animation, int animationIdx, const Ref<ModelAsset>& owner)
	{
		SetAnimation(animation);
		SetAnimationIdx(animationIdx);
		SetOwnerModel(owner);
	}

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

	const Ref<ModelAsset>& AnimationAsset::GetOwnerModel() const
	{
		return m_OwnerModel;
	}

	void AnimationAsset::SetOwnerModel(const Ref<ModelAsset>& ownerMesh)
	{
		m_OwnerModel = ownerMesh;
	}

	void AnimationAsset::SerializeData(YAML::Emitter& out)
	{
		out << YAML::Key << c_AnimationIndexKey << YAML::Value << m_AnimationIdx;
		SerializeAsset(out, c_OwnerMeshKey, m_OwnerModel);
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

		m_OwnerModel = DeserializeAsset<ModelAsset>(node[c_OwnerMeshKey]);
		if (m_OwnerModel == nullptr)
		{
			Log::CoreWarning("Failed to deserialize owner mesh of animation in {0}",
				m_Path.string());
		}

		if (m_AnimationIdx == -1 || m_OwnerModel == nullptr)
			return;

		try
		{
			//SetAnimation(AnimationLoader::LoadAnimation(m_OwnerModel->GetPath(), m_OwnerModel, m_AnimationIdx));
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to load animation in {0} with animation index: {1}. Error: {2}",
				m_Path.string(), m_AnimationIdx, e.what());
		}
	}
}
