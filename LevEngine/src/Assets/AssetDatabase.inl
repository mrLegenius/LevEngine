#pragma once
#include "AssetDatabase.h"

namespace LevEngine
{
    template <class T, class ...Args>
    Ref<T> AssetDatabase::CreateNewAsset(const Path& path, Args ...args)
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        if (exists(path))
        {
            Log::CoreError("Asset already exists in {}", path);
            return GetAsset<T>(path);
        }
			
        const auto uuid = UUID();

        auto asset = CreateRef<T>(path, uuid, eastl::forward<Args>(args)...);
        asset->Serialize();

        m_Assets.emplace(uuid, asset);
        m_AssetsByPath.emplace(path, asset);

        return asset;
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
    Vector<Ref<T>> AssetDatabase::GetAllAssetsOfClass()
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must be an asset");

        Vector<Ref<T>> foundAssets;
        for (const auto& asset : m_Assets)
        {
            if (auto assetT = CastRef<T>(asset.second))
            {
                foundAssets.push_back(assetT);
            }
        }

        return foundAssets;
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
