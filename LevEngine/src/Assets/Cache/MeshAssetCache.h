#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class Mesh;

    class LEV_API MeshAssetCache
    {
    public:
        static Ref<Mesh> LoadFromCache(UUID assetUUID);
        static void SaveToCache(UUID assetUUID, const Ref<Mesh>& mesh);
    };
}


