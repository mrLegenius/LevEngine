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
        
        static void LoadModelHierarchy(const Path& path, const aiNode* node, ModelNode* parent,
                                       Matrix accTransform,
                                       const aiScene* scene);
        static Ref<MeshAsset> CreateMeshAsset(const Path& path, String name, const Ref<Mesh>& mesh);

        static Vector<Ref<MeshAsset>> LoadMeshes(const Path& path, const aiScene* scene);
        static Vector<Ref<MaterialAsset>> LoadMaterials(const Path& path, const aiScene* scene);

        static Ref<Mesh> LoadModel(const Path& path);
        static Ref<Mesh> ParseMesh(const aiMesh* mesh);
        static Ref<Mesh> ParseMesh(const aiMesh* mesh, Matrix cumulativeTransform);

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
