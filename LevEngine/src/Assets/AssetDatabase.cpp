﻿#include "levpch.h"
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
}
