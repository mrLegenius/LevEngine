#pragma once
#include "Assets/Asset.h"
#include "Assets/AssetDatabase.h"

namespace LevEngine
{
class ResourceManager
{
public:
    inline static const String ResourcesDatabaseFilename = "ResourcesDatabase.asset";
    
    template<class T>
    static Ref<T> LoadAsset(const String& address)
    {
        static_assert(eastl::is_base_of_v<Asset, T>, "T must derive from Asset");
        
        const auto it = m_AddressToUUIDMap.find(address);
        if (it == m_AddressToUUIDMap.end())
        {
            Log::CoreError("Failed to load asset with '{0}' address", address);
            return nullptr;
        }

        const auto uuid = it->second;

        m_AssetsCache[uuid];
        
        return AssetDatabase::GetAsset<T>(uuid);
    }
    
    static void Init(const Path& projectPath);

    //TODO: Maybe move this to LevEditor
    static void Build(const Path& projectPath);
    static void ChangeAddress(const Ref<Asset>& asset, const String& newAddress);

private:
    inline static Map<String, UUID> m_AddressToUUIDMap;

    inline static Map<UUID, Weak<Asset>> m_AssetsCache;
};
}
