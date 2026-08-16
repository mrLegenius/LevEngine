#include "levpch.h"
#include "ResourceManager.h"

#include "Project.h"
#include "Assets/MissingReferences.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
void ResourceManager::Init(const Path& projectPath)
{
    try
    {
        Path path = projectPath / ResourcesDatabaseFilename.c_str();

        if (!exists(path)) return;

        YAML::Node data = YAML::LoadFile(path.string());

        if (!data["Assets"]) return;

        auto assets = data["Assets"];

        for (auto asset : assets)
        {
            UUID uuid = asset["UUID"].as<uint64_t>();
            auto address = asset["Address"].as<String>();

            m_AddressToUUIDMap.emplace(address, uuid);

            // An address whose asset was deleted stays in the database, because the scripts naming
            // that address are still there and dropping the entry would only move the failure. It is
            // reported as the dangling reference it is instead, and the editor can remove it.
            if (!AssetDatabase::HasAsset(uuid))
            {
                MissingReferences::Report(uuid, MissingReferences::ResourcesDatabaseSource,
                    Format("Address '{0}'", address));
                continue;
            }

            if (const auto assetToAddress = AssetDatabase::GetAsset(uuid))
                assetToAddress->SetAddress(address);
        }
    }
    catch (std::exception& e)
    {
        Log::CoreWarning(e.what());
    }
}

void ResourceManager::Build(const Path& projectPath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "Assets" << YAML::Value;
    
    out << YAML::BeginSeq;

    for (auto [address, uuid] : m_AddressToUUIDMap)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "UUID" << YAML::Value << uuid;
        out << YAML::Key << "Address" << YAML::Value << address;
        out << YAML::EndMap;
    }
    
    out << YAML::EndSeq;
    out << YAML::EndMap;
    
    try
    {
        std::ofstream fout((projectPath / ResourcesDatabaseFilename.c_str()).string().c_str());
        fout << out.c_str();
    }
    catch (std::exception& e)
    {
        Log::CoreWarning("Failed to serialize resources database. Error: {0}", e.what());
    }
}

bool ResourceManager::RemoveAddress(const String& address)
{
    return m_AddressToUUIDMap.erase(address) > 0;
}

Vector<Pair<String, UUID>> ResourceManager::GetDanglingAddresses()
{
    Vector<Pair<String, UUID>> dangling;

    for (const auto& [address, uuid] : m_AddressToUUIDMap)
    {
        if (AssetDatabase::HasAsset(uuid)) continue;

        dangling.emplace_back(address, uuid);
    }

    return dangling;
}

void ResourceManager::ChangeAddress(const Ref<Asset>& asset, const String& newAddress)
{
    if (asset->GetAddress() == newAddress) return;

    const auto it = m_AddressToUUIDMap.find(newAddress);
    if (it != m_AddressToUUIDMap.end())
    {
        const auto uuid = it->second;
        if (const auto busyAsset = AssetDatabase::GetAsset(uuid))
        {
            auto relativePath = relative(busyAsset->GetPath(), AssetDatabase::GetAssetsPath()).string();
            Log::CoreWarning("Failed to set address for '{0}'. '{1}' belongs to '{2}'",
                busyAsset->GetName(), newAddress, relativePath);
            return;
        }

        m_AddressToUUIDMap.erase(newAddress);
    }

    m_AddressToUUIDMap.erase(asset->GetAddress());
    m_AddressToUUIDMap.emplace(newAddress, asset->GetUUID());

    asset->SetAddress(newAddress);
}
}
