#include "levpch.h"
#include "ModelParser.h"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

#include "AssimpConverter.h"
#include "Assets/AssetDatabase.h"
#include "Assets/MeshAsset.h"
#include "Assets/ModelNode.h"
#include "Renderer/3D/Mesh.h"

namespace LevEngine
{
    
    //TODO: Add aiProcess_GlobalScale
    unsigned int importFlags
        = aiProcess_CalcTangentSpace
        | aiProcess_Triangulate
        | aiProcess_RemoveRedundantMaterials
        | aiProcess_JoinIdenticalVertices
        | aiProcess_GenSmoothNormals
        | aiProcess_GenUVCoords
        //| aiProcess_SortByPType // Do not split mesh to sub meshes with different primitives types
        //| aiProcess_FlipWindingOrder
        | aiProcess_ImproveCacheLocality // It may help with rendering large models
        //| aiProcess_FixInfacingNormals //May help to fix inwards normals but can screw up double side faces
        | aiProcess_OptimizeMeshes
        | aiProcess_OptimizeGraph
        | aiProcess_GlobalScale 
    ;

    ModelImportResult ModelParser::Load(const Path& path, ModelImportParameters params)
    {
        ModelImportResult result{};

        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
        importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, params.scale);
        const aiScene* scene = importer.ReadFile(path.string(), importFlags);

        ModelNode* resultHierarchy = new ModelNode();
        resultHierarchy->Name = path.filename().string().c_str();
        resultHierarchy->Transform = Matrix::Identity;
        resultHierarchy->MeshUUID = 0;
        
        LoadModelHierarchy(path, scene->mRootNode, resultHierarchy, Matrix::Identity, scene);
        result.Hierarchy = resultHierarchy;

        return result;
    }

    void ModelParser::LoadModelHierarchy(const Path& path, const aiNode* node, ModelNode* parent, Matrix accTransform, const aiScene* scene)
    {
        const Matrix currentNodeTransform = AssimpConverter::ToMatrix(node->mTransformation, true);
        ModelNode* result{};
        
        for (int i = 0; i < node->mNumMeshes > 0; ++i)
        {
            result = new ModelNode();
            
            auto meshIndex = node->mMeshes[i];
            auto meshData = scene->mMeshes[meshIndex];

            auto mesh = ParseMesh(meshData, currentNodeTransform);

            constexpr uint32_t maxNameLength = 64;
            String fileName = String(meshData->mName.C_Str()).substr(0, maxNameLength);
            auto meshAsset = CreateMeshAsset(path, fileName, mesh);
            
            result->MeshUUID = meshAsset->GetUUID();

            result->Transform = currentNodeTransform;
            result->Name = meshData->mName.C_Str();

            if (node->mNumMeshes > 1)
                result->Name.append(ToString(i+1));
            
            parent->Children.push_back(result);
        }
        
        if (node->mNumChildren && node->mNumMeshes > 1)
        {
            result = new ModelNode();
            result->MeshUUID = 0;
            result->Transform = currentNodeTransform;
            result->Name = node->mName.C_Str();
        }

        parent = result ? result : parent;

        Matrix transform = AssimpConverter::ToMatrix(node->mTransformation, false) * accTransform;
        
        for (uint32_t i = 0; i < node->mNumChildren; ++i)
            LoadModelHierarchy(path, node->mChildren[i], parent, transform, scene);
    }

    Ref<MeshAsset> ModelParser::CreateMeshAsset(const Path& path, String name, const Ref<Mesh>& mesh)
    {
        const auto modelDirectory = path.parent_path();
        const auto meshesDirectory = modelDirectory / "Meshes";

        if (!exists(meshesDirectory))
            create_directory(meshesDirectory);
        
        name.append(".mesh");
        Path meshPath = meshesDirectory / name.c_str();

        if (AssetDatabase::AssetExists(meshPath))
            AssetDatabase::DeleteAsset(AssetDatabase::GetAsset<MeshAsset>(meshPath));
            
        Ref<MeshAsset> meshAsset = AssetDatabase::CreateNewAsset<MeshAsset>(meshPath, mesh);

        return meshAsset;
    }
    
    Vector<Ref<MeshAsset>> ModelParser::LoadMeshes(const Path& path, const aiScene* scene)
    {
        const auto modelDirectory = path.parent_path();
        const auto meshesDirectory = modelDirectory / "Meshes";

        if (!exists(meshesDirectory))
            create_directory(meshesDirectory);
        
        Vector<Ref<MeshAsset>> meshes;
        for (int i = 0; i < scene->mNumMeshes; ++i)
        {
            auto meshData = scene->mMeshes[i];

            constexpr uint32_t maxNameLength = 64;
            String fileName = String(meshData->mName.C_Str()).substr(0, maxNameLength);
            fileName.append(".mesh");
            Path meshPath = meshesDirectory / fileName.c_str();

            if (AssetDatabase::AssetExists(meshPath))
                AssetDatabase::DeleteAsset(AssetDatabase::GetAsset<MeshAsset>(meshPath));
            
            Ref<MeshAsset> meshAsset = AssetDatabase::CreateNewAsset<MeshAsset>(meshPath, ParseMesh(meshData));

            meshes.push_back(meshAsset);
        }

        return meshes;
    }

    Ref<Mesh> ModelParser::LoadModel(const Path& path)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path.string(), importFlags);

        Ref<Mesh> resultMesh = CreateRef<Mesh>();
		
        if (scene == nullptr)
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

    Ref<Mesh> ModelParser::ParseMesh(const aiMesh* mesh)
    {
        Ref<Mesh> newMesh = CreateRef<Mesh>();
	
        const size_t nvertices = mesh->mNumVertices;
        const aiVector3D* vertices = mesh->mVertices;
        newMesh->ResizeBoneArrays(nvertices);

        for (size_t vertexIdx = 0; vertexIdx < nvertices; vertexIdx++)
        {
            const auto point = AssimpConverter::ToVector3(vertices[vertexIdx]);

            const auto uv = AssimpConverter::ToVector2(mesh->mTextureCoords[0][vertexIdx]);
            const auto normal = AssimpConverter::ToVector3(mesh->mNormals[vertexIdx]);
            const auto tangent = AssimpConverter::ToVector3(mesh->mTangents[vertexIdx]);

            newMesh->AddVertex(point);
            newMesh->AddUV(uv);
            newMesh->AddNormal(normal);
            newMesh->AddTangent(tangent);
        }

        ExtractBoneWeightForVertices(newMesh, mesh);

        const size_t nFaces = mesh->mNumFaces;
        const aiFace* meshFaces = mesh->mFaces;
        for (size_t faceIdx = 0; faceIdx < nFaces; faceIdx++)
        {
            newMesh->AddIndex(meshFaces[faceIdx].mIndices[0]);
            newMesh->AddIndex(meshFaces[faceIdx].mIndices[1]);
            newMesh->AddIndex(meshFaces[faceIdx].mIndices[2]);
        }

        newMesh->Init();
        newMesh->GenerateAABBBoundingVolume();

        return newMesh;
    }

    Ref<Mesh> ModelParser::ParseMesh(const aiMesh* mesh, Matrix cumulativeTransform)
    {
        Ref<Mesh> newMesh = CreateRef<Mesh>();
	
        const size_t nvertices = mesh->mNumVertices;
        const aiVector3D* vertices = mesh->mVertices;
        newMesh->ResizeBoneArrays(nvertices);

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

            newMesh->AddVertex(point);
            newMesh->AddUV(uv);
            newMesh->AddNormal(normal);
            newMesh->AddTangent(tangent);
        }

        ExtractBoneWeightForVertices(newMesh, mesh);

        const size_t nFaces = mesh->mNumFaces;
        const aiFace* meshFaces = mesh->mFaces;
        for (size_t faceIdx = 0; faceIdx < nFaces; faceIdx++)
        {
            newMesh->AddIndex(meshFaces[faceIdx].mIndices[0]);
            newMesh->AddIndex(meshFaces[faceIdx].mIndices[1]);
            newMesh->AddIndex(meshFaces[faceIdx].mIndices[2]);
        }

        newMesh->Init();
        newMesh->GenerateAABBBoundingVolume();
        
        newMesh->NormalizeBoneWeights();

        return newMesh;
    }

    void ModelParser::ParseMesh(const aiNode* node, const aiScene* scene, Ref<Mesh>& resultMesh,
        Matrix cumulativeTransform)
    {
        const Matrix currentNodeTransform = AssimpConverter::ToMatrix(node->mTransformation, true);
        cumulativeTransform *= currentNodeTransform;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            const uint32_t meshIdx = node->mMeshes[i];
            Ref<Mesh> newMesh = CreateRef<Mesh>();
			
            const aiMesh* mesh = scene->mMeshes[meshIdx];
	
            const size_t nvertices = mesh->mNumVertices;
            const aiVector3D* vertices = mesh->mVertices;
            newMesh->ResizeBoneArrays(nvertices);

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

                newMesh->AddVertex(point);
                newMesh->AddUV(uv);
                newMesh->AddNormal(normal);
                newMesh->AddTangent(tangent);
            }

            ExtractBoneWeightForVertices(newMesh, mesh);

            const size_t nFaces = mesh->mNumFaces;
            const aiFace* meshFaces = mesh->mFaces;
            for (size_t faceIdx = 0; faceIdx < nFaces; faceIdx++)
            {
                newMesh->AddIndex(meshFaces[faceIdx].mIndices[0]);
                newMesh->AddIndex(meshFaces[faceIdx].mIndices[1]);
                newMesh->AddIndex(meshFaces[faceIdx].mIndices[2]);
            }

            newMesh->Init();
            newMesh->GenerateAABBBoundingVolume();
			
            resultMesh->AddSubMesh(newMesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            ParseMesh(node->mChildren[i], scene, resultMesh, cumulativeTransform);
        }

        resultMesh->NormalizeBoneWeights();
    }

    void ModelParser::ExtractBoneWeightForVertices(const Ref<Mesh>& resultMesh, const aiMesh* mesh)
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
                const unsigned int vertexId = weights[weightIndex].mVertexId;
                const float weight = weights[weightIndex].mWeight;
                LEV_CORE_ASSERT(vertexId < resultMesh->GetVerticesCount())
                resultMesh->AddBoneWeight(vertexId, boneID, weight);
            }
        }
    }
}
