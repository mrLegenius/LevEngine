#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    struct ModelNode;
    
    class LEV_API ModelAssetCache
    {
    public:
        static ModelNode* LoadFromCache(UUID assetUUID);
        static void SaveToCache(UUID assetUUID, const ModelNode* data);
    };
}
