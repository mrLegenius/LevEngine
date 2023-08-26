#include "levpch.h"
#include <queue>

#include "AssetDatabase.h"

namespace LevEngine
{
	void AssetDatabase::ProcessAllAssets()
	{
		std::queue<std::filesystem::path> directories;
		directories.push(AssetsRoot);
		do
		{
			auto directory = directories.front();
			directories.pop();

			for (auto& directoryEntry : std::filesystem::directory_iterator(directory))
			{
				if (directoryEntry.is_directory())
					directories.push(directoryEntry.path());
				else if (directoryEntry.path().extension() != ".meta")
				{
					auto uuid = UUID();
					auto filepath = directoryEntry.path();
					auto pathString = directoryEntry.path().string();
					pathString.append(".meta");

					bool needToGenerateMeta = false;
					if (std::filesystem::exists(pathString))
					{
						YAML::Node data = YAML::LoadFile(pathString);
						if (auto uuidData = data["UUID"])
						{
							uuid = uuidData.as<uint64_t>();
						}
						else
						{
							needToGenerateMeta = true;
						}
					}
					else 
					{
						needToGenerateMeta = true;
					}

					Ref<Asset> asset = CreateAsset(filepath, uuid);

					if (needToGenerateMeta)
						asset->SerializeMeta();

					m_Assets.emplace(uuid, asset);
					m_AssetsByPath.emplace(filepath, asset);
				}
			}
		} while (!directories.empty());
	}

	void AssetDatabase::RenameAsset(const Ref<Asset>& asset, const std::string& name)
	{
		if (asset->GetName() == name) return;

		const auto directory = asset->GetPath().parent_path();
		const auto newPath = directory / (name + asset->GetExtension());

		m_AssetsByPath.erase(asset->GetPath());
		std::filesystem::rename(asset->GetPath(), newPath);
		std::filesystem::rename(
			asset->GetPath().string().append(".meta"), 
			newPath.string().append(".meta"));

		asset->Rename(newPath);
		m_AssetsByPath.emplace(newPath, asset);
	}
}
