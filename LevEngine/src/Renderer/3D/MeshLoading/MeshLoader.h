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
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_JoinIdenticalVertices
			| aiProcess_GenSmoothNormals
			//| aiProcess_SortByPType // Do not split mesh to sub meshes with different primitives types
			//| aiProcess_FlipWindingOrder
			| aiProcess_ImproveCacheLocality // It may help with rendering large models
			//| aiProcess_FixInfacingNormals //May help to fix inwards normals but can screw up double side faces
		);

		Ref<Mesh> resultMesh = CreateRef<Mesh>();
		
		if (nullptr == scene)
		{
			Log::CoreWarning("Failed to load mesh in {0}. Returning empty mesh", path.string());
			return resultMesh;
		}

		auto& rootNode = scene->mRootNode;

		if (scene->mNumMeshes == 0)
		{
			Log::CoreWarning("There is no mesh in file {0}. Returning empty mesh", path.string());
			return resultMesh;
		}

		ParseMesh(rootNode, scene, resultMesh, Matrix::Identity);
		
		resultMesh->Init();
		resultMesh->GenerateAABBBoundingVolume();

		return resultMesh;
	}

	static void ParseMesh(const aiNode* node, const aiScene* scene, Ref<Mesh>& resultMesh, Matrix cumulativeTransform)
	{
		const Matrix currentNodeTransform = AssimpConverter::ToMatrix(node->mTransformation, true);
		cumulativeTransform *= currentNodeTransform;

		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			const int meshIdx = node->mMeshes[i];
			const aiMesh* mesh = scene->mMeshes[meshIdx];
	
			const size_t nvertices = mesh->mNumVertices;
			const aiVector3D* vertices = mesh->mVertices;
			const uint32_t firstVertexId = resultMesh->GetVerticesCount(); // offset to first vertex in current aiMesh
			resultMesh->ResizeBoneArrays(firstVertexId + nvertices);

			for (size_t vertexIdx = 0; vertexIdx < nvertices; vertexIdx++)
			{
				const aiVector3D vertex = vertices[vertexIdx];
				
				const auto point = Vector3::Transform(
					Vector3(vertex.x, vertex.y, vertex.z), cumulativeTransform);

				const auto uv = Vector2(mesh->mTextureCoords[0][vertexIdx].x, mesh->mTextureCoords[0][vertexIdx].y);
				const auto normal = Vector3::Transform(AssimpConverter::ToVector3(mesh->mNormals[vertexIdx]),
					cumulativeTransform);
				const auto tangent = Vector3::Transform(AssimpConverter::ToVector3(mesh->mTangents[vertexIdx]),
					cumulativeTransform);

				resultMesh->AddVertex(point);
				resultMesh->AddUV(uv);
				resultMesh->AddNormal(normal);
				resultMesh->AddTangent(tangent);
			}

			ExtractBoneWeightForVertices(resultMesh, mesh, firstVertexId);

			const size_t nFaces = mesh->mNumFaces;
			const aiFace* meshFaces = mesh->mFaces;
			for (size_t faceIdx = 0; faceIdx < nFaces; faceIdx++)
			{
				resultMesh->AddIndex(meshFaces[faceIdx].mIndices[0] + firstVertexId);
				resultMesh->AddIndex(meshFaces[faceIdx].mIndices[1] + firstVertexId);
				resultMesh->AddIndex(meshFaces[faceIdx].mIndices[2] + firstVertexId);
			}
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ParseMesh(node->mChildren[i], scene, resultMesh, cumulativeTransform);
		}

		resultMesh->NormalizeBoneWeights();
	}

	/*
	 * <params>
	 * firstVertexId: ID offset to a first vertex in current aiMesh.
	 * Assimp stores meshes in hierarchy, but we flatten them to one mesh, so we require offsets.
	 * </params>
	 */
	static void ExtractBoneWeightForVertices(const Ref<Mesh>& resultMesh, const aiMesh* mesh, const uint32_t firstVertexId)
	{
		for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID;
			String boneName = mesh->mBones[boneIndex]->mName.C_Str();
			UnorderedMap<String, BoneInfo>& boneInfoMap = resultMesh->GetBoneInfoMap();

			if (!boneInfoMap.count(boneName))
			{
				BoneInfo newBoneInfo;
				int boneCount = resultMesh->GetBoneCount();

				newBoneInfo.id = boneCount;
				newBoneInfo.offset = AssimpConverter::ToMatrix(mesh->mBones[boneIndex]->mOffsetMatrix, false);
				
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
				const unsigned int vertexId = weights[weightIndex].mVertexId + firstVertexId;
				const float weight = weights[weightIndex].mWeight;
				LEV_CORE_ASSERT(vertexId < resultMesh->GetVerticesCount())
				resultMesh->AddBoneWeight(vertexId, boneID, weight);
			}
		}
	}
};
}
