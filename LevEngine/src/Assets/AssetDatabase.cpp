#include "levpch.h"

#include "AssetDatabase.h"

#include "DataTypes/Queue.h"

namespace LevEngine
{
	void AssetDatabase::ImportAsset(const Path& path)
	{
		if (!path.has_extension()) return;

		auto uuid = UUID();
		auto pathString = path.string();
		pathString.append(".meta");

		bool needToGenerateMeta = false;
		if (std::filesystem::exists(pathString))
		{
			YAML::Node data = YAML::LoadFile(pathString);
			if (const auto uuidData = data["UUID"])
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

		Ref<Asset> asset = CreateAsset(path, uuid);

		if (needToGenerateMeta)
			asset->SerializeMeta();

		m_Assets.try_emplace(uuid, asset);
		m_AssetsByPath.try_emplace(path, asset);
	}

	void AssetDatabase::ProcessAllAssets()
	{
		m_AssetsByPath.clear();
		m_Assets.clear();
		Queue<Path> directories;
		directories.push(GetAssetsPath());
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
					ImportAsset(path);
			}
		} while (!directories.empty());
	}

	void AssetDatabase::RenameAsset(const Ref<Asset>& asset, const String& name)
	{
		if (asset->GetName() == name) return;

		const auto directory = asset->GetPath().parent_path();
		const auto newPath = directory / (name + asset->GetExtension()).c_str();

		m_AssetsByPath.erase(asset->GetPath());
		std::filesystem::rename(asset->GetPath(), newPath);
		std::filesystem::rename(
			asset->GetPath().string().append(".meta").c_str(), 
			newPath.string().append(".meta").c_str());

		asset->Rename(newPath);
		m_AssetsByPath.emplace(newPath, asset);
	}

	void AssetDatabase::MoveAsset(const Ref<Asset>& asset, const Path& directory)
	{
		const auto currentDirectory = asset->GetPath().parent_path();

		if (currentDirectory == directory) return;
		if (directory.has_extension()) return;

		const auto oldPath = asset->GetPath();
		const auto newPath = directory / asset->GetFullName().c_str();

		if (exists(newPath))
		{
			Log::CoreWarning("Failed to move asset. Asset '{0}' already exists in {1}", asset->GetName(), directory);
			return;
		}

		try
		{
			std::filesystem::rename(oldPath, newPath);
			std::filesystem::rename(
				oldPath.string().append(".meta"),
				newPath.string().append(".meta"));
		}
		catch (std::filesystem::filesystem_error& e)
		{
			Log::CoreWarning("Failed to move asset. Error: {0}", e.what());
			return;
		}

		asset->Rename(newPath);
		m_AssetsByPath.emplace(newPath, asset);
		m_AssetsByPath.erase(oldPath);
	}

	void AssetDatabase::DeleteAsset(const Ref<Asset>& asset)
	{
		if (asset == nullptr) return;

		const auto path = asset->GetPath();
		const auto uuid = asset->GetUUID();

		std::filesystem::remove(path);
		std::filesystem::remove(path.string().append(".meta"));

		m_AssetsByPath.erase(path);
		m_Assets.erase(uuid);
	}
}
