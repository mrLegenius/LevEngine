#pragma once

namespace LevEngine
{
    class Mesh;

    class MeshAssetCache
    {
    public:
        static Ref<Mesh> LoadFromCache(UUID assetUUID);
        static void SaveToCache(UUID assetUUID, const Ref<Mesh>& mesh);
    };
}


