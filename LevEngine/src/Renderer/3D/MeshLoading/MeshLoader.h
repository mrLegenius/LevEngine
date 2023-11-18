#pragma once


#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

#include "DataTypes/Path.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/3D/MeshLoading/AssimpConverter.h"
#include "Kernel/Asserts.h"

namespace LevEngine
{
class MeshLoader
{
public:
	static Ref<Mesh> LoadMesh(const Path& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(),
			aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_PreTransformVertices //Combine to single mesh if possible
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_JoinIdenticalVertices
			| aiProcess_GenSmoothNormals
			//| aiProcess_SortByPType // Do not split mesh to sub meshes with different primitives types
			//| aiProcess_FlipWindingOrder
			//| aiProcess_ImproveCacheLocality // It may help with rendering large models
			| aiProcess_FixInfacingNormals //May help to fix inwards normals
			| aiProcess_GenBoundingBoxes
		);

		Ref<Mesh> resultMesh = CreateRef<Mesh>();

		if (nullptr == scene)
		{
			Log::CoreWarning("Failed to load mesh in {0}. Returning empty mesh", path.string());
			return resultMesh;
		}

		auto& rootNode = scene->mRootNode;

		if (rootNode->mNumMeshes == 0)
		{
			Log::CoreWarning("There is no mesh in file {0}. Returning empty mesh", path.string());
			return resultMesh;
		}

		if (rootNode->mNumMeshes > 2)
		{
			Log::CoreWarning("Multiple mesh is not supported. File: {0}", path.string());
		}

		const aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[0]];

		const size_t nvertices = mesh->mNumVertices;
		const aiVector3D* vertices = mesh->mVertices;

		for (size_t i = 0; i < nvertices; i++)
		{
			const aiVector3D vertex = vertices[i];

			resultMesh->SetVertexBoneDataToDefault(static_cast<int>(i));

			const auto point = Vector3(vertex.x, vertex.y, vertex.z);

			const auto uv = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			const auto normal = AssimpConverter::ToVector3(mesh->mNormals[i]);
			const auto tangent = AssimpConverter::ToVector3(mesh->mTangents[i]);
			const auto biTangent = AssimpConverter::ToVector3(mesh->mNormals[i]);

			resultMesh->AddVertex(point);
			resultMesh->AddUV(uv);
			resultMesh->AddNormal(normal);
			resultMesh->AddTangent(tangent);
			resultMesh->AddBiTangent(biTangent);
		}

		ExtractBoneWeightForVertices(resultMesh, mesh, scene);

		const size_t nFaces = mesh->mNumFaces;
		const aiFace* meshFaces = mesh->mFaces;
		for (size_t i = 0; i < nFaces; i++)
		{
			resultMesh->AddIndex(meshFaces[i].mIndices[0]);
			resultMesh->AddIndex(meshFaces[i].mIndices[1]);
			resultMesh->AddIndex(meshFaces[i].mIndices[2]);
		}

		const auto aabb = mesh->mAABB;
		const auto aabbMin = AssimpConverter::ToVector3(aabb.mMin);
		const auto aabbMax = AssimpConverter::ToVector3(aabb.mMax);
		resultMesh->SetAABBBoundingVolume(aabbMin, aabbMax);
		
		resultMesh->Init();

		return resultMesh;
	}

	static void ExtractBoneWeightForVertices(Ref<Mesh>& resultMesh, const aiMesh* mesh, const aiScene* scene)
	{
		for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			String boneName = mesh->mBones[boneIndex]->mName.C_Str();
			UnorderedMap<String, BoneInfo>& boneInfoMap = resultMesh->GetBoneInfoMap();

			if (!boneInfoMap.count(boneName))
			{
				BoneInfo newBoneInfo;
				int boneCount = resultMesh->GetBoneCount();

				newBoneInfo.id = boneCount;
				newBoneInfo.offset = AssimpConverter::ToMatrix(mesh->mBones[boneIndex]->mOffsetMatrix);

				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;

				resultMesh->SetBoneCount(boneCount);
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}

			LEV_CORE_ASSERT(boneID != -1)

			const auto weights = mesh->mBones[boneIndex]->mWeights;
			const unsigned int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (unsigned int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				const unsigned int vertexId = weights[weightIndex].mVertexId;
				const float weight = weights[weightIndex].mWeight;
				LEV_CORE_ASSERT(vertexId <= resultMesh->GetVerticesCount())
				resultMesh->SetVertexBoneData(vertexId, boneID, weight);
			}
		}
	}
};
}
