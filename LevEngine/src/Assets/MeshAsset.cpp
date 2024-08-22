#include "levpch.h"
#include "MeshAsset.h"

#include "AnimationAsset.h"
#include "EngineAssets.h"
#include "Cache/MeshAssetCache.h"

namespace LevEngine
{
    MeshAsset::MeshAsset(const Path& path, const UUID uuid, const Ref<Mesh>& mesh)
        : Asset(path, uuid), m_Mesh(mesh)
    {
        SaveToCache();
    }

    Ref<Texture> MeshAsset::GetIcon() const
    {
        return Icons::Mesh();
    }

    bool MeshAsset::LoadFromCache()
    {
        m_Mesh = MeshAssetCache::LoadFromCache(m_UUID);
        return m_Mesh != nullptr;;
    }

    void MeshAsset::SaveToCache()
    {
        if (m_Mesh)
            MeshAssetCache::SaveToCache(m_UUID, m_Mesh);
    }
}
