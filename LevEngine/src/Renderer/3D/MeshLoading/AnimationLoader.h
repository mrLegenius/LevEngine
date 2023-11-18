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
			const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			Vector<Ref<Animation>> animations;

			for (unsigned int animationIdx = 0; animationIdx < scene->mNumAnimations; ++animationIdx)
			{
				animations.push_back(ReadAnimation(scene, mesh, animationIdx));
			}

			return animations;
		}

		static Ref<Animation> LoadAnimation(const Path& path, Ref<Mesh>& mesh, int animationIdx)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			return ReadAnimation(scene, mesh, animationIdx);
		}

	private:
		static Ref<Animation> ReadAnimation(const aiScene* scene, Ref<Mesh>& mesh, int animationIdx)
		{
			const aiAnimation* animation = scene->mAnimations[animationIdx];

			Ref<Animation> resultAnimation = CreateRef<Animation>();

			resultAnimation->m_Duration = animation->mDuration;
			resultAnimation->m_TicksPerSecond = animation->mTicksPerSecond;
			ReadHeirarchyData(resultAnimation, resultAnimation->m_RootNode, scene->mRootNode);
			ReadMissingBones(resultAnimation, animation, mesh);

			return resultAnimation;
		}

		static void ReadHeirarchyData(Ref<Animation>& resultAnimation, NodeData& dest, const aiNode* src)
		{
			LEV_CORE_ASSERT(src);

			dest.name = src->mName.data;
			dest.transformation = AssimpConverter::ToMatrix(src->mTransformation);
			dest.childrenCount = src->mNumChildren;

			for (unsigned int i = 0; i < src->mNumChildren; i++)
			{
				NodeData newData;
				ReadHeirarchyData(resultAnimation, newData, src->mChildren[i]);
				dest.children.push_back(newData);
			}
		}

		static void ReadMissingBones(Ref<Animation>& resultAnimation, const aiAnimation* animation, Ref<Mesh>& mesh)
		{
			int size = animation->mNumChannels;

			UnorderedMap<String, BoneInfo>& boneInfoMap = mesh->GetBoneInfoMap();//getting m_BoneInfoMap from Model class
			int& boneCount = mesh->GetBoneCount(); //getting the m_BoneCounter from Model class

			//reading channels(bones engaged in an animation and their keyframes)
			for (int i = 0; i < size; i++)
			{
				auto channel = animation->mChannels[i];
				String boneName = channel->mNodeName.data;

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