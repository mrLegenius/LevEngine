#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

#include "DataTypes/Path.h"
#include "Renderer/3D/Mesh.h"

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

			const auto point = Vector3(vertex.x, vertex.y, vertex.z);

			const auto uv = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			const auto normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			const auto tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			const auto biTangent = Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

			resultMesh->AddVertex(point);
			resultMesh->AddUV(uv);
			resultMesh->AddNormal(normal);
			resultMesh->AddTangent(tangent);
			resultMesh->AddBiTangent(biTangent);
		}

		const size_t nFaces = mesh->mNumFaces;
		const aiFace* meshFaces = mesh->mFaces;
		for (size_t i = 0; i < nFaces; i++)
		{
			resultMesh->AddIndex(meshFaces[i].mIndices[0]);
			resultMesh->AddIndex(meshFaces[i].mIndices[1]);
			resultMesh->AddIndex(meshFaces[i].mIndices[2]);
		}

		const auto aabb = mesh->mAABB;
		const auto aabbMin = Vector3{aabb.mMin.x, aabb.mMin.y, aabb.mMin.z};
		const auto aabbMax = Vector3{aabb.mMax.x, aabb.mMax.y, aabb.mMax.z};
		resultMesh->SetAABBBoundingVolume(aabbMin, aabbMax);
		
		resultMesh->Init();

		return resultMesh;
	}
};
}
