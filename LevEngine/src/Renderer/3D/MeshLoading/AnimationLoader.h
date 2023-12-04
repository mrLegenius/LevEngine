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

		static Ref<Animation> LoadAnimation(const Path& path, const Ref<Mesh>& mesh, int animationIdx)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path.string(),
				aiProcess_CalcTangentSpace
				| aiProcess_Triangulate
				| aiProcess_RemoveRedundantMaterials
				| aiProcess_JoinIdenticalVertices
				| aiProcess_GenSmoothNormals
				//| aiProcess_SortByPType // Do not split mesh to sub meshes with different primitives types
				//| aiProcess_FlipWindingOrder
				//| aiProcess_ImproveCacheLocality // It may help with rendering large models
				| aiProcess_FixInfacingNormals //May help to fix inwards normals
			);

			LEV_CORE_ASSERT(scene && scene->mRootNode);

			return ReadAnimation(scene, mesh, animationIdx);
		}

	private:
		static Ref<Animation> ReadAnimation(const aiScene* scene, const Ref<Mesh>& mesh, int animationIdx)
		{
			const aiAnimation* animation = scene->mAnimations[animationIdx];

			Ref<Animation> resultAnimation = CreateRef<Animation>();

			resultAnimation->m_Duration = animation->mDuration;
			resultAnimation->m_TicksPerSecond = animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0;
			resultAnimation->m_Name = animation->mName.C_Str();
			NodeData* rootNode = resultAnimation->m_RootNode;
			ReadHeirarchyData(resultAnimation, rootNode, scene->mRootNode);
			ReadMissingBones(resultAnimation, animation, mesh);

			resultAnimation->m_RootInverseTransform = rootNode->localTransform.Invert();

			return resultAnimation;
		}

		static void ReadHeirarchyData(const Ref<Animation>& resultAnimation, NodeData* dest, const aiNode* src)
		{
			LEV_CORE_ASSERT(src);

			dest->name = src->mName.data;
			dest->originalTransform = AssimpConverter::ToMatrix(src->mTransformation, true);
			dest->localTransform = dest->originalTransform;

			for (unsigned int i = 0; i < src->mNumChildren; i++)
			{
				NodeData* newData = new NodeData();
				newData->parent = dest;
				ReadHeirarchyData(resultAnimation, newData, src->mChildren[i]);
				dest->children.emplace_back(newData);
			}
		}

		static void ReadMissingBones(const Ref<Animation>& resultAnimation, const aiAnimation* animation, const Ref<Mesh>& mesh)
		{
			const unsigned int size = animation->mNumChannels;

			UnorderedMap<String, BoneInfo>& boneInfoMap = mesh->GetBoneInfoMap();//getting m_BoneInfoMap from Model class
			int& boneCount = mesh->GetBoneCount(); //getting the m_BoneCounter from Model class

			//reading channels(bones engaged in an animation and their keyframes)
			for (unsigned int i = 0; i < size; i++)
			{
				const auto channel = animation->mChannels[i];
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