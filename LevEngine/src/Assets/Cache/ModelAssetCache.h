#pragma once

namespace LevEngine
{
    struct ModelNode;
    
    class ModelAssetCache
    {
    public:
        static ModelNode* LoadFromCache(UUID assetUUID);
        static void SaveToCache(UUID assetUUID, const ModelNode* data);
    };
}
