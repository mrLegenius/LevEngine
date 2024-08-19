#include "levpch.h"
#include "ModelParser.h"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>

#include "AssimpConverter.h"
#include "Assets/AssetDatabase.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MaterialPBRAsset.h"
#include "Assets/MeshAsset.h"
#include "Assets/ModelNode.h"
#include "Assets/TextureAsset.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/Material/MaterialPBR.h"

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
        | aiProcess_SortByPType // Do not split mesh to sub meshes with different primitives types
        //| aiProcess_FlipWindingOrder
        | aiProcess_ImproveCacheLocality // It may help with rendering large models
        //| aiProcess_FixInfacingNormals //May help to fix inwards normals but can screw up double side faces
        | aiProcess_OptimizeMeshes
        | aiProcess_OptimizeGraph
        | aiProcess_GlobalScale;

    ModelImportResult ModelParser::Load(const Path& path, ModelImportParameters params)
    {
        ModelImportResult result{};

        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
        importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, params.scale);
        const aiScene* scene = importer.ReadFile(path.string(), importFlags);

        ModelNode* resultHierarchy = new ModelNode();
        resultHierarchy->Name = path.filename().stem().string().c_str();
        resultHierarchy->Transform = Matrix::Identity;
        resultHierarchy->MeshUUID = 0;
        resultHierarchy->MaterialUUID = 0;

        auto materials = LoadMaterials(path, scene);
        LoadModelHierarchy(path, scene->mRootNode, resultHierarchy, Matrix::Identity, scene, materials);
        result.Hierarchy = resultHierarchy;

        return result;
    }

    void ModelParser::LoadModelHierarchy(const Path& path, const aiNode* node, ModelNode* parent, Matrix accTransform,
                                         const aiScene* scene, const Vector<Ref<MaterialAsset>>& materialAssets)
    {
        const Matrix currentNodeTransform = AssimpConverter::ToMatrix(node->mTransformation, true);
        ModelNode* result{};

        for (int i = 0; i < node->mNumMeshes > 0; ++i)
        {
            result = new ModelNode();

            auto meshIndex = node->mMeshes[i];
            auto meshData = scene->mMeshes[meshIndex];

            auto materialIndex = meshData->mMaterialIndex;

            auto mesh = ParseMesh(meshData, currentNodeTransform);

            auto fileName = AssimpConverter::ToName(meshData->mName);
            auto meshAsset = CreateMeshAsset(path, fileName, mesh);

            result->MeshUUID = meshAsset->GetUUID();
            result->MaterialUUID = materialAssets[materialIndex]->GetUUID();

            result->Transform = currentNodeTransform;
            result->Name = fileName;

            if (node->mNumMeshes > 1)
                result->Name.append(ToString(i + 1));

            parent->Children.push_back(result);
        }

        if (node->mNumChildren && node->mNumMeshes > 1)
        {
            result = new ModelNode();
            result->MeshUUID = 0;
            result->MaterialUUID = 0;
            result->Transform = currentNodeTransform;
            result->Name = AssimpConverter::ToName(node->mName);
        }

        parent = result ? result : parent;

        Matrix transform = AssimpConverter::ToMatrix(node->mTransformation, false) * accTransform;

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
            LoadModelHierarchy(path, node->mChildren[i], parent, transform, scene, materialAssets);
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

    Vector<Ref<MaterialAsset>> ModelParser::LoadMaterials(const Path& path, const aiScene* scene)
    {
        const auto modelDirectory = path.parent_path();
        const auto directory = modelDirectory / "Materials";

        if (!exists(directory))
            create_directory(directory);

        Vector<Ref<MaterialAsset>> materials;
        for (int i = 0; i < scene->mNumMaterials; ++i)
        {
            auto data = scene->mMaterials[i];

            auto fileName = AssimpConverter::ToName(data->GetName());
            fileName.append(".pbr");
            Path assetPath = directory / fileName.c_str();

            if (AssetDatabase::AssetExists(assetPath))
                AssetDatabase::DeleteAsset(AssetDatabase::GetAsset<MaterialAsset>(assetPath));

            Ref<MaterialPBRAsset> asset = AssetDatabase::CreateNewAsset<MaterialPBRAsset>(
                assetPath, ParseMaterial(data));

            AssignMaterialTextures(path, data, asset);
            asset->Serialize();

            materials.push_back(asset);
        }

        return materials;
    }

    void ModelParser::AssignMaterialTextures(const Path& path, const aiMaterial* material, const Ref<MaterialPBRAsset>& asset)
    {
        const auto modelDirectory = path.parent_path();
        aiString colorTexture, metallicTexture, roughnessTexture, normalTexture, aoTexture, emissiveTexture;
        material->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &colorTexture);
        material->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &metallicTexture);
        material->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &roughnessTexture);
        material->GetTexture(aiTextureType_NORMALS, 0, &normalTexture);
        material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aoTexture);
        material->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTexture);

        if (!colorTexture.length)
            material->GetTexture(aiTextureType_DIFFUSE, 0, &colorTexture);
        
        if (colorTexture.length)
            asset->SetTexture(MaterialPBR::TextureType::Albedo, AssetDatabase::GetAsset<TextureAsset>(modelDirectory / colorTexture.C_Str()));

        if (metallicTexture.length)
            asset->SetTexture(MaterialPBR::TextureType::Metallic, AssetDatabase::GetAsset<TextureAsset>(modelDirectory / metallicTexture.C_Str()));

        if (roughnessTexture.length)
            asset->SetTexture(MaterialPBR::TextureType::Roughness, AssetDatabase::GetAsset<TextureAsset>(modelDirectory / roughnessTexture.C_Str()));

        if (normalTexture.length)
            asset->SetTexture(MaterialPBR::TextureType::Normal, AssetDatabase::GetAsset<TextureAsset>(modelDirectory / normalTexture.C_Str()));

        if (aoTexture.length)
            asset->SetTexture(MaterialPBR::TextureType::AmbientOcclusion, AssetDatabase::GetAsset<TextureAsset>(modelDirectory / aoTexture.C_Str()));

        if (emissiveTexture.length)
            asset->SetTexture(MaterialPBR::TextureType::Emissive, AssetDatabase::GetAsset<TextureAsset>(modelDirectory / emissiveTexture.C_Str()));
    }

    MaterialPBR ModelParser::ParseMaterial(const aiMaterial* material)
    {
        MaterialPBR newMaterial{};

        aiColor4D color(1.f, 1.f, 1.f, 1.f);
        if (material->Get(AI_MATKEY_BASE_COLOR, color) == aiReturn_FAILURE)
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        
        newMaterial.SetTintColor(AssimpConverter::ToColor(color));

        float metallicFactor = 0.001f;
        material->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor);
        newMaterial.SetMetallic(metallicFactor);

        float roughnessFactor = 1.f - 0.001f;
        material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor);
        newMaterial.SetRoughness(roughnessFactor);
        
        return newMaterial;
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
