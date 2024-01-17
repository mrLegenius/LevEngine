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

namespace LevEngine
{
	class AnimationLoader
	{
	public:
		static Vector<Ref<Animation>> LoadAllAnimations(const Path& path, Ref<Mesh>& mesh)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string(), aiProcess_FindInvalidData);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			Vector<Ref<Animation>> animations;

			for (unsigned int animationIdx = 0; animationIdx < scene->mNumAnimations; ++animationIdx)
			{
				animations.push_back(ReadAnimation(scene, mesh, animationIdx));
			}

			return animations;
		}

		static Ref<Animation> LoadAnimation(const Path& path, const Ref<Mesh>& mesh, int animationIdx)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string(), aiProcess_FindInvalidData);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			return ReadAnimation(scene, mesh, animationIdx);
		}

	private:
		static Ref<Animation> ReadAnimation(const aiScene* scene, const Ref<Mesh>& resultMesh, int animationIdx)
		{
			const aiAnimation* animation = scene->mAnimations[animationIdx];

			Ref<Animation> resultAnimation = CreateRef<Animation>();

			resultAnimation->m_Duration = animation->mDuration;
			resultAnimation->m_TicksPerSecond = animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0;
			resultAnimation->m_Name = animation->mName.C_Str();
			NodeData* rootNode = resultAnimation->m_RootNode;
			ReadHeirarchyData(resultAnimation, rootNode, scene->mRootNode);
			ReadKeyframes(resultAnimation, animation, resultMesh);

			return resultAnimation;
		}

		static void ReadHeirarchyData(const Ref<Animation>& resultAnimation, NodeData* dest, const aiNode* src)
		{
			LEV_CORE_ASSERT(src);

			dest->name = src->mName.data;
			dest->boneBindPoseTransform = AssimpConverter::ToMatrix(src->mTransformation, false);
			dest->boneCurrentTransform = dest->boneBindPoseTransform;

			for (unsigned int i = 0; i < src->mNumChildren; i++)
			{
				NodeData* newData = new NodeData();
				newData->parent = dest;
				ReadHeirarchyData(resultAnimation, newData, src->mChildren[i]);
				dest->children.emplace_back(newData);
			}
		}

		static void ReadKeyframes(const Ref<Animation>& resultAnimation,
			const aiAnimation* aiAnimation, const Ref<Mesh>& resultMesh)
		{
			const unsigned int numChannels = aiAnimation->mNumChannels;

			UnorderedMap<String, BoneInfo>& boneInfoMap = resultMesh->GetBoneInfoMap();
			int& boneCount = resultMesh->GetBoneCount();

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