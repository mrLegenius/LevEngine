#pragma once

#include "assimp/scene.h"

#include "Assets/ModelAsset.h"
#include "Renderer/3D/Animation.h"
#include "Renderer/3D/MeshLoading/AssimpConverter.h"

#include "Renderer/3D/BoneInfo.h"
#include "Math/Math.h"

namespace LevEngine
{
	class AnimationLoader
	{
	public:
		static Vector<Ref<Animation>> LoadAllAnimations(const aiScene* scene, UnorderedMap<String, BoneInfo>& boneInfoMap, int& boneCount)
		{
			Vector<Ref<Animation>> animations;

			for (unsigned int animationIdx = 0; animationIdx < scene->mNumAnimations; ++animationIdx)
			{
				animations.push_back(ReadAnimation(scene, scene->mAnimations[animationIdx], boneInfoMap, boneCount));
			}

			return animations;
		}

	private:
		static Ref<Animation> ReadAnimation(const aiScene* scene, const aiAnimation* animation, UnorderedMap<String, BoneInfo>& boneInfoMap, int& boneCount)
		{
			Ref<Animation> resultAnimation = CreateRef<Animation>();

			resultAnimation->m_Duration = animation->mDuration;
			resultAnimation->m_TicksPerSecond = Math::IsZero(animation->mTicksPerSecond) ? 25.0 : animation->mTicksPerSecond;
			resultAnimation->m_Name = AssimpConverter::ToName(animation->mName);
			NodeData* rootNode = resultAnimation->m_RootNode;
			ReadHierarchyData(resultAnimation, rootNode, scene->mRootNode);
			ReadKeyframes(resultAnimation, animation, boneInfoMap, boneCount);

			return resultAnimation;
		}

		static void ReadHierarchyData(const Ref<Animation>& resultAnimation, NodeData* dest, const aiNode* src)
		{
			LEV_CORE_ASSERT(src);

			dest->name = src->mName.data;
			dest->boneBindPoseTransform = AssimpConverter::ToMatrix(src->mTransformation, false);
			dest->boneCurrentTransform = dest->boneBindPoseTransform;

			for (unsigned int i = 0; i < src->mNumChildren; i++)
			{
				NodeData* newData = new NodeData();
				newData->parent = dest;
				ReadHierarchyData(resultAnimation, newData, src->mChildren[i]);
				dest->children.emplace_back(newData);
			}
		}

		static void ReadKeyframes(const Ref<Animation>& resultAnimation,
			const aiAnimation* aiAnimation, UnorderedMap<String, BoneInfo>& boneInfoMap, int& boneCount)
		{
			const unsigned int numChannels = aiAnimation->mNumChannels;

			for (unsigned int channelIdx = 0; channelIdx < numChannels; channelIdx++)
			{
				const auto channel = aiAnimation->mChannels[channelIdx];
				const String& boneName = channel->mNodeName.data;

				if (boneInfoMap.count(boneName) == 0)
				{
					boneInfoMap.insert({ boneName, BoneInfo{boneCount, Matrix::Identity} });
					boneCount++;
				}

				resultAnimation->m_Bones.push_back(Bone(channel->mNodeName.data,
					boneInfoMap.find(channel->mNodeName.data)->second.id, channel));
			}

			resultAnimation->m_BoneInfoMap = boneInfoMap;
		}
	};
}