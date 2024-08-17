#pragma once

struct aiMesh;
struct aiScene;
struct aiNode;

namespace LevEngine
{
    class MeshAsset;
    struct ModelNode;
    class Mesh;

    struct ModelImportResult
    {
        Vector<Ref<MeshAsset>> Meshes;
        ModelNode* Hierarchy;
    };

    struct ModelImportParameters
    {
        float scale = 1;
    };
    
    class ModelParser
    {
    public:
        static ModelImportResult Load(const Path& path, ModelImportParameters params);
        
        static ModelNode* LoadModelHierarchy(const aiNode* node, ModelNode* parent, Matrix accTransform,
            const aiScene* scene,
            const Vector<Ref<MeshAsset>>& meshAssets);
        
        static Vector<Ref<MeshAsset>> LoadMeshes(const Path& path, const aiScene* scene);

        static Ref<Mesh> LoadModel(const Path& path);
        static Ref<Mesh> ParseMesh(const aiMesh* mesh);

        static void ParseMesh(const aiNode* node, const aiScene* scene, Ref<Mesh>& resultMesh,
                              Matrix cumulativeTransform);

        /*
         * <params>
         * firstVertexId: ID offset to a first vertex in current aiMesh.
         * Assimp stores meshes in hierarchy, but we flatten them to one mesh, so we require offsets.
         * </params>
         */
        static void ExtractBoneWeightForVertices(const Ref<Mesh>& resultMesh, const aiMesh* mesh);
    };
}
