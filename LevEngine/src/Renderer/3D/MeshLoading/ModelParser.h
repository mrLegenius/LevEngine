#pragma once
#include <assimp/material.h>

#include "AI/Components/AIAgentCrowdComponent.h"
#include "Assets/MaterialPBRAsset.h"
#include "Renderer/Material/MaterialPBR.h"
#include "Renderer/3D/BoneInfo.h"

struct aiMesh;
struct aiScene;
struct aiNode;

namespace LevEngine
{
    class Animation;
    struct BoneInfo;
    class MaterialAsset;
    class MeshAsset;
    struct ModelNode;
    class Mesh;

    struct ModelImportResult
    {
        Vector<Ref<MeshAsset>> Meshes;
        ModelNode* Hierarchy;
        UnorderedMap<String, BoneInfo> boneInfoMap{};
        int boneCount = 0;
        Vector<Ref<Animation>> Animations{};
    };

    struct ModelImportParameters
    {
        float scale = 1;
    };
    
    class ModelParser
    {
    public:
        static ModelImportResult Load(const Path& path, ModelImportParameters params);
        
        static void LoadModelHierarchy(const Path& path, const aiNode* node, ModelNode* parent,
                                       const Matrix& accTransform,
                                       const aiScene* scene, const Vector<Ref<MaterialAsset>>& materialAssets, UnorderedMap<String, BoneInfo>& boneInfoMap, int
                                       & boneCount);
        static Ref<MeshAsset> CreateMeshAsset(const Path& path, String name, const Ref<Mesh>& mesh);

        static Vector<Ref<MaterialAsset>> LoadMaterials(const Path& path, const aiScene* scene);
        static void AssignMaterialTextures(const Path& path, const aiMaterial* material, const Ref<MaterialPBRAsset>& asset);

        static MaterialPBR ParseMaterial(const aiMaterial* material);
        static Ref<Mesh> ParseMesh(const aiMesh* mesh, const Matrix& cumulativeTransform, UnorderedMap<String, BoneInfo>& boneInfoMap, int& boneCount);

        /*
         * <params>
         * firstVertexId: ID offset to a first vertex in current aiMesh.
         * Assimp stores meshes in hierarchy, but we flatten them to one mesh, so we require offsets.
         * </params>
         */
        static void ExtractBoneWeightForVertices(const aiMesh* mesh, const Ref<Mesh>& resultMesh, UnorderedMap<String, BoneInfo>& boneInfoMap, int& boneCount);
    };
}
