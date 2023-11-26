#pragma once
#include "AssetDatabase.h"

namespace LevEngine
{
    template <class T>
    Ref<T> AssetDatabase::CreateAsset(const Path& path)
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        if (exists(path))
        {
            Log::CoreError("Asset already exists in {}", path);
            return nullptr;
        }
			
        const auto uuid = UUID();

        auto asset = CreateAsset(path, uuid);
        asset->Serialize();

        String metaPath = path.string().c_str();
        metaPath += ".meta";

        CreateMeta(metaPath.c_str(), uuid);

        m_Assets.emplace(uuid, asset);
        m_AssetsByPath.emplace(path, asset);

        return CastRef<T>(asset);
    }

    template <class T>
    Ref<T> AssetDatabase::GetAsset(const UUID uuid)
    {
        static_assert(eastl::is_base_of_v<Asset, T>, "T must be an asset");

        return GetAsset<T>(GetAsset(uuid));
    }

    template <class T>
    Ref<T> AssetDatabase::GetAsset(const Path& path)
    {
        static_assert(eastl::is_base_of_v<Asset, T>, "T must be an asset");

        return GetAsset<T>(GetAsset(path));
    }

    template <class T>
    Ref<T> AssetDatabase::GetAsset(const Ref<Asset>& asset)
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must be an asset");
        if (!asset) return nullptr;

        auto assetT = CastRef<T>(asset);
        if (!assetT)
        {
            Log::CoreWarning("Asset ({0}) in {1} is not {2}", asset->GetUUID(), asset->GetPath(), typeid(T).name());
            return nullptr;
        }

        return assetT;
    }
}
