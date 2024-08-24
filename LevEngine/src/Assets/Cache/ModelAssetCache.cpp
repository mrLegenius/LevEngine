#include "levpch.h"
#include "ModelAssetCache.h"

#include "Assets/AssetDatabase.h"
#include "Assets/ModelNode.h"
#include "BinaryIO/BinaryReader.h"
#include "BinaryIO/BinaryWriter.h"

namespace LevEngine
{
    ModelNode* LoadModel(BinaryReader& reader)
    {
        ModelNode* node = new ModelNode();

        node->MeshUUID = reader.Read<uint64_t>();
        node->MaterialUUID = reader.Read<uint64_t>();
        node->Name = reader.ReadString();
        node->Transform = reader.Read<Matrix>();

        auto childrenCount = reader.Read<size_t>();

        for (int i = 0; i < childrenCount; ++i)
        {
            auto child = LoadModel(reader);
            node->Children.push_back(child);
        }
        
        return node;
    }
    
    ModelNode* ModelAssetCache::LoadFromCache(UUID assetUUID)
    {
        LEV_PROFILE_FUNCTION();

        auto cachePath = AssetDatabase::GetAssetCachePath(assetUUID);

        if (!exists(cachePath)) return nullptr;

        try
        {
            BinaryReader reader{cachePath};

            return LoadModel(reader);
        }
        catch (std::exception& e)
        {
            Log::CoreError("Failed to load model from cache. Error {0}", e.what());
        }

        return nullptr;
    }

    void SaveModel(const ModelNode* data, BinaryWriter& writer)
    {
        writer.Write(data->MeshUUID);
        writer.Write(data->MaterialUUID);
        writer.WriteString(data->Name);
        writer.Write(data->Transform);
        writer.Write(data->Children.size());

        for (auto child : data->Children)
            SaveModel(child, writer);
    }

    void ModelAssetCache::SaveToCache(UUID assetUUID, const ModelNode* data)
    {
        LEV_PROFILE_FUNCTION();

        try
        {
            auto cachePath = AssetDatabase::GetAssetCachePath(assetUUID);

            BinaryWriter writer{cachePath};

            SaveModel(data, writer);
        }
        catch (std::exception& e)
        {
            Log::CoreError("Failed to save model to cache. Error {0}", e.what());
        }
    }
}
