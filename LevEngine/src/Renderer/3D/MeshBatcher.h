#pragma once

#include "Kernel/Core.h"
#include "Renderer/Renderer3D.h"

namespace LevEngine
{
    class Material;
    class Mesh;
    struct Frustum;
    struct Transform;

    // Every instance of one mesh drawn with one material, ready to be issued as a single
    // instanced draw call.
    struct LEV_API MeshBatch
    {
        Ref<Mesh> Geometry;
        Material* SurfaceMaterial{};
        Vector<MeshInstanceData> Instances;
    };

    // Groups meshes by (mesh, material) so a scene made of thousands of copies of a handful of
    // meshes collapses into a handful of draw calls.
    //
    // A batcher is meant to be owned by a pass and reused every frame: Clear() empties the
    // batches but keeps their storage, so a scene whose contents settle stops allocating.
    class LEV_API MeshBatcher
    {
    public:
        // Adds mesh and all of its sub-meshes at transform. Sub-meshes inherit the transform and
        // the material, and batch under their own key. A non-null frustum culls per mesh, exactly
        // like the non-instanced path does.
        void Add(const Ref<Mesh>& mesh, Material* material, const Transform& transform, const Frustum* frustum);

        void Clear();

        // Batches left empty by the last Clear() are kept as spare storage -- skip the ones with
        // no instances.
        [[nodiscard]] const Vector<MeshBatch>& GetBatches() const { return m_Batches; }

    private:
        MeshBatch& GetOrCreateBatch(const Ref<Mesh>& mesh, Material* material);

        Vector<MeshBatch> m_Batches;
        Map<Pair<const Mesh*, const Material*>, size_t> m_BatchIndices;
        size_t m_UsedBatches{};
    };
}
