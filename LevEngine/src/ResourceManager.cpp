#include "levpch.h"
#include "ResourceManager.h"

#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
void ResourceManager::Init(const Path& projectPath)
{
    try
    {
        YAML::Node data = YAML::LoadFile((projectPath / ResourcesDatabaseFilename.c_str()).string().c_str());

        if (!data["Assets"]) return;

        auto assets = data["Assets"];

        for (auto asset : assets)
        {
            UUID uuid = asset["UUID"].as<uint64_t>();
            auto address = asset["Address"].as<String>();

            m_AddressToUUIDMap.emplace(address, uuid);
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

    Queue<Path> directories;
    directories.push(AssetDatabase::GetAssetsPath());
    do
    {
        auto directory = directories.front();
        directories.pop();

        for (auto& directoryEntry : std::filesystem::directory_iterator(directory))
        {
            auto path = directoryEntry.path();

            if (directoryEntry.is_directory())
                directories.push(path);
            else if (path.extension() != ".meta")
            {
                const auto& asset = AssetDatabase::GetAsset<Asset>(path, false);
                if (asset && !asset->GetAddress().empty())
                {
                    auto relativePath = relative(asset->GetPath(), AssetDatabase::GetAssetsPath());
                    
                    out << YAML::BeginMap;
                    out << YAML::Key << "UUID" << YAML::Value << asset->GetUUID();
                    out << YAML::Key << "Address" << YAML::Value << asset->GetAddress().c_str();
                    out << YAML::EndMap;
                }
            }
        }
    } while (!directories.empty());
    
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
}
