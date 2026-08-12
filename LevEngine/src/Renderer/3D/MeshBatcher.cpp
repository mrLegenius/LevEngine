#include "levpch.h"
#include "MeshBatcher.h"

#include "Mesh.h"
#include "Math/Frustum.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    void MeshBatcher::Add(const Ref<Mesh>& mesh, Material* material, const Transform& transform,
                          const Frustum* frustum)
    {
        LEV_PROFILE_FUNCTION();

        if (!mesh) return;

        MeshInstanceData instance{};
        bool instanceCalculated = false;

        Queue<Ref<Mesh>> meshesToBatch;
        meshesToBatch.push(mesh);

        while (meshesToBatch.size() > 0)
        {
            auto current = meshesToBatch.front();
            meshesToBatch.pop();

            for (const auto& subMesh : current->GetSubMeshes())
            {
                if (subMesh)
                    meshesToBatch.push(subMesh);
            }

            if (!current->IndexBuffer) continue;
            if (frustum && !current->IsOnFrustum(*frustum, transform)) continue;

            //<--- The inverse transpose is the expensive part, so it is only paid once an entity
            //     actually contributes something to a batch ---<<
            if (!instanceCalculated)
            {
                const auto& model = transform.GetModel();
                instance = {model, model.Transpose().Invert()};
                instanceCalculated = true;
            }

            GetOrCreateBatch(current, material).Instances.push_back(instance);
        }
    }

    void MeshBatcher::Clear()
    {
        LEV_PROFILE_FUNCTION();

        for (auto& batch : m_Batches)
        {
            //<--- clear(), not a fresh vector: the instance storage is what we want to keep ---<<
            batch.Instances.clear();
            batch.Geometry = nullptr;
            batch.SurfaceMaterial = nullptr;
        }

        m_BatchIndices.clear();
        m_UsedBatches = 0;
    }

    MeshBatch& MeshBatcher::GetOrCreateBatch(const Ref<Mesh>& mesh, Material* material)
    {
        const auto key = MakePair(static_cast<const Mesh*>(mesh.get()), static_cast<const Material*>(material));

        const auto it = m_BatchIndices.find(key);
        if (it != m_BatchIndices.end())
            return m_Batches[it->second];

        if (m_UsedBatches == m_Batches.size())
            m_Batches.emplace_back();

        const size_t index = m_UsedBatches++;

        auto& batch = m_Batches[index];
        batch.Geometry = mesh;
        batch.SurfaceMaterial = material;

        m_BatchIndices.emplace(key, index);

        return batch;
    }
}
