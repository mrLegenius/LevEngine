#pragma once
#include "DataTypes/Path.h"
#include "Renderer/3D/Animation.h"
#include "Renderer/3D/Bone.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/3D/MeshLoading/AssimpConverter.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "Kernel/Asserts.h"
#include "Renderer/3D/Skeleton.h"

namespace LevEngine
{
	class AnimationLoader
	{
	public:
		static Vector<Ref<Animation>> LoadAllAnimations(const Path& path, const Ref<Skeleton>& resultSkeleton)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string(), aiProcess_FindInvalidData);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			Vector<Ref<Animation>> animations;

			for (unsigned int animationIdx = 0; animationIdx < scene->mNumAnimations; ++animationIdx)
			{
				animations.push_back(ReadAnimation(scene, resultSkeleton, animationIdx));
			}

			return animations;
		}

		static Ref<Animation> LoadAnimation(const Path& path, const Ref<Skeleton>& resultSkeleton, int animationIdx)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string(), aiProcess_FindInvalidData);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			return ReadAnimation(scene, resultSkeleton, animationIdx);
		}

	private:
		static Ref<Animation> ReadAnimation(const aiScene* scene, const Ref<Skeleton>& resultSkeleton, int animationIdx)
		{
			const aiAnimation* animation = scene->mAnimations[animationIdx];

			Ref<Animation> resultAnimation = CreateRef<Animation>();

			resultAnimation->m_Duration = animation->mDuration;
			resultAnimation->m_TicksPerSecond = animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0;
			resultAnimation->m_Name = animation->mName.C_Str();
			
			SkeletonNodeData* rootNode = nullptr;
			
			ReadHeirarchyData(resultAnimation, rootNode, scene->mRootNode);
			ReadKeyframes(resultAnimation, animation, resultSkeleton);
			
			resultSkeleton->SetRootNode(rootNode);

			return resultAnimation;
		}

		static void ReadHeirarchyData(const Ref<Animation>& resultAnimation, SkeletonNodeData* dest, const aiNode* src)
		{
			LEV_CORE_ASSERT(src);

			dest->name = src->mName.data;
			dest->boneBindPoseTransform = AssimpConverter::ToMatrix(src->mTransformation, false);

			for (unsigned int i = 0; i < src->mNumChildren; i++)
			{
				SkeletonNodeData* newData = new SkeletonNodeData();
				newData->parent = dest;
				ReadHeirarchyData(resultAnimation, newData, src->mChildren[i]);
				dest->children.emplace_back(newData);
			}
		}

		static void ReadKeyframes(const Ref<Animation>& resultAnimation,
			const aiAnimation* aiAnimation, const Ref<Skeleton>& resultSkeleton)
		{
			const unsigned int numChannels = aiAnimation->mNumChannels;

			UnorderedMap<String, BoneInfo>& boneInfoMap = resultSkeleton->GetBoneInfoMap();
			const int boneCount = boneInfoMap.size();

			for (unsigned int channelIdx = 0; channelIdx < numChannels; channelIdx++)
			{
				const auto channel = aiAnimation->mChannels[channelIdx];
				const String& boneName = channel->mNodeName.data;

				if (boneInfoMap.count(boneName) == 0)
				{
					boneInfoMap.insert({ boneName, BoneInfo{boneCount, Matrix::Identity} });
				}

				resultAnimation->m_Bones.push_back(Bone(channel->mNodeName.data,
					boneInfoMap.find(channel->mNodeName.data)->second.id, channel));
			}
		}
	};
}
