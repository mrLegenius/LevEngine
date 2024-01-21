#include "levpch.h"

#include "AssetDatabase.h"

#include "AnimationAsset.h"
#include "AudioBankAsset.h"
#include "DefaultAsset.h"
#include "MaterialPBRAsset.h"
#include "MaterialSimpleAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "Project.h"
#include "SceneAsset.h"
#include "ScriptAsset.h"
#include "SkyboxAsset.h"
#include "TextureAsset.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	Path AssetDatabase::GetAssetsPath()
	{
		if (Project::GetProject())
			return Project::GetRoot() / AssetsRoot;

		return AssetsRoot;
	}

	Path AssetDatabase::GetLibraryPath()
	{
		if (Project::GetProject())
			return Project::GetRoot() / LibraryRoot;

		return LibraryRoot;
	}

	Path AssetDatabase::GetRelativePath(const Path& path)
	{
		return relative(GetAssetsPath(), path);
	}

	Path AssetDatabase::GetLibraryPath(const Path& path)
	{
		const auto pathString = path.string();
		const size_t assetsRootIdx = pathString.rfind(AssetsRoot.string());
		return Path(pathString.substr(0, assetsRootIdx)
			+ LibraryRoot.string()
			+ pathString.substr(assetsRootIdx
			+ AssetsRoot.string().length())
			+ ".meta");
	}

	void AssetDatabase::ImportAsset(const Path& path, bool needToGenerateLibrary)
	{
		if (path.extension() == ".meta") return;

		auto uuid = UUID();
		const auto pathString = path.string();
		
		auto metaPathString = pathString;
		String address;
		metaPathString.append(".meta");

		bool needToGenerateMeta = false;
		if (std::filesystem::exists(metaPathString))
		{
			YAML::Node data = YAML::LoadFile(metaPathString);
			if (const auto uuidData = data["UUID"])
			{
				uuid = uuidData.as<uint64_t>();
			}
			else
			{
				needToGenerateMeta = true;
			}

			if (const auto addressData = data["Address"])
			{
				address = addressData.as<String>();
			}
		}
		else 
		{
			needToGenerateMeta = true;
		}

		const auto libraryPath = GetLibraryPath(pathString);
		if (!std::filesystem::exists(libraryPath))
		{
			needToGenerateLibrary = true;
		}

		Ref<Asset> asset = CreateAsset(path, uuid);
		if (CastRef<ScriptAsset>(asset))
		{
			asset->Deserialize();
		}

		if (needToGenerateMeta)
			asset->SerializeMeta();

		if (needToGenerateLibrary)
		{
			asset->SerializeLibrary();
		}

		if (!address.empty())
			asset->SetAddress(address);
		
		m_Assets.try_emplace(uuid, asset);
		m_AssetsByPath.try_emplace(path, asset);
	}

	void AssetDatabase::ProcessAllAssets()
	{
		if (!std::filesystem::exists(GetAssetsPath()))
			create_directory(GetAssetsPath());
		
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

				ImportAsset(path);
			}
		} while (!directories.empty());
	}

	bool AssetDatabase::IsAssetTexture(const Path& path)
	{
		const auto extension = path.extension().string();

		return
			extension == ".png"
			|| extension == ".jpg"
			|| extension == ".jpeg"
			|| extension == ".tga"
			|| extension == ".hdr"
			|| extension == ".psd"
		;
	}

	bool AssetDatabase::IsAssetMesh(const Path& path)
	{
		const auto extension = path.extension().string();

		return
			extension == ".obj"
			|| extension == ".fbx"
			|| extension == ".dae"
			|| extension == ".gltf"
			|| extension == ".glb"
			|| extension == ".blend"
			|| extension == ".3ds"
			|| extension == ".ase"
			|| extension == ".ifc"
			|| extension == ".xgl"
			|| extension == ".zgl"
			|| extension == ".ply"
			|| extension == ".dxf"
			|| extension == ".lwo"
			|| extension == ".lws"
			|| extension == ".lxo"
			|| extension == ".stl"
			|| extension == ".x"
			|| extension == ".ac"
			|| extension == ".ms3d"
			;
	}

	bool AssetDatabase::IsAssetMaterial(const Path& path)
	{
		const auto extension = path.extension().string();

		return extension == ".mat";
	}

	bool AssetDatabase::IsAssetPBRMaterial(const Path& path)
	{
		const auto extension = path.extension().string();

		return extension == ".pbr";
	}

	bool AssetDatabase::IsAssetSkybox(const Path& path)
	{
		const auto extension = path.extension().string();

		return extension == ".skybox";
	}

	bool AssetDatabase::IsAssetPrefab(const Path& path)
	{
		const auto extension = path.extension().string();

		return extension == ".prefab";
	}

	bool AssetDatabase::IsAssetScene(const Path& path)
	{
		const auto extension = path.extension().string();

		return extension == ".scene";
	}

	bool AssetDatabase::IsAssetScript(const Path& path)
	{
		const auto extension = path.extension().string();

		return extension == ".lua";
	}

	bool AssetDatabase::IsAssetAudioBank(const Path& path)
	{
		const String pathString = ToString(path);
		const auto index = pathString.find('.');
			
		if (index == eastl::string::npos) return false;

		const auto extension = pathString.right(pathString.length() - index);

		return extension == ".bank";
	}

	bool AssetDatabase::IsAssetAnimationClip(const Path& path)
	{
		const auto extension = path.extension().string();
		return extension == ".anim";
	}

	Ref<Asset> AssetDatabase::CreateAsset(const Path& path, UUID uuid)
	{
		if (IsAssetTexture(path))
			return CreateRef<TextureAsset>(path, uuid);

		if (IsAssetMaterial(path))
			return CreateRef<MaterialSimpleAsset>(path, uuid);

		if (IsAssetPBRMaterial(path))
			return CreateRef<MaterialPBRAsset>(path, uuid);
			
		if (IsAssetSkybox(path))
			return CreateRef<SkyboxAsset>(path, uuid);

		if (IsAssetMesh(path))
			return CreateRef<MeshAsset>(path, uuid);

		if (IsAssetPrefab(path))
			return CreateRef<PrefabAsset>(path, uuid);

		if (IsAssetScene(path))
			return CreateRef<SceneAsset>(path, uuid);

		if (IsAssetAudioBank(path))
			return CreateRef<AudioBankAsset>(path, uuid);

		if (IsAssetAnimationClip(path))
			return CreateRef<AnimationAsset>(path, uuid);

		if (IsAssetScript(path))
			return CreateRef<ScriptAsset>(path, uuid);

		return CreateRef<DefaultAsset>(path, uuid);
	}

	void AssetDatabase::CreateFolder(const Path& path)
	{
		if (!std::filesystem::exists(path))
			create_directory(path);
	}

	Ref<Asset> AssetDatabase::GetAsset(const Path& path, const bool deserialize)
	{
		const auto assetIt = m_AssetsByPath.find(path);

		if (assetIt == m_AssetsByPath.end()) return nullptr;

		const auto& asset = assetIt->second;
		if (!asset->IsDeserialized() && deserialize)
			asset->Deserialize();

		return asset;
	}

	Ref<Asset> AssetDatabase::GetAsset(const UUID uuid)
	{
		const auto assetIt = m_Assets.find(uuid);
		if (assetIt == m_Assets.end())
		{
			Log::CoreWarning("Asset with {0} is not found", static_cast<std::uint64_t>(uuid));
			return nullptr;
		}

		const auto& asset = assetIt->second;
		if (!asset->IsDeserialized())
			asset->Deserialize();

		return asset;
	}

	void AssetDatabase::RenameAsset(const Ref<Asset>& asset, const String& name)
	{
		if (asset->GetName() == name) return;

		const auto oldPath = asset->GetPath();
		const auto directory = oldPath.parent_path();
		const auto newPath = directory / (name + asset->GetExtension()).c_str();

		m_AssetsByPath.erase(oldPath);

		if (is_directory(newPath))
		{
			for (std::filesystem::recursive_directory_iterator i(newPath), end; i != end; ++i)
			{
				m_AssetsByPath.erase(i->path());
			}
		}

		if (exists(oldPath))
		{
			std::filesystem::rename(oldPath, newPath);
		}

		const auto oldMetaPath = oldPath.string().append(".meta").c_str();
		if (exists(oldPath))
		{
			std::filesystem::rename(oldMetaPath, newPath.string().append(".meta").c_str());
		}

		asset->Rename(newPath);
		m_AssetsByPath.emplace(newPath, asset);

		DeleteAllAssetsInDirectory(oldPath);
		ReimportAllAssetsInDirectory(newPath);
	}

	void AssetDatabase::MoveAsset(const Ref<Asset>& asset, const Path& directory)
	{
		if (asset == nullptr) return;

		const auto currentDirectory = asset->GetPath().parent_path();

		if (currentDirectory == directory) return;
		if (!is_directory(directory)) return;

		const auto oldPath = asset->GetPath();
		const auto newPath = directory / asset->GetFullName().c_str();

		if (exists(newPath))
		{
			Log::CoreWarning("Failed to move asset. Asset '{0}' already exists in {1}", asset->GetName(), GetRelativePath(directory));
			return;
		}

		try
		{
			std::filesystem::rename(oldPath, newPath);

			const auto oldMetaPath = oldPath.string().append(".meta");
			std::filesystem::rename(oldMetaPath, newPath.string().append(".meta"));
		}
		catch (std::filesystem::filesystem_error& e)
		{
			Log::CoreWarning("Failed to move asset. Error: {0}", e.what());
			return;
		}

		asset->Rename(newPath);
		m_AssetsByPath.emplace(newPath, asset);
		m_AssetsByPath.erase(oldPath);

		DeleteAllAssetsInDirectory(oldPath);
		ReimportAllAssetsInDirectory(newPath);
	}

	void AssetDatabase::DeleteAsset(const Ref<Asset>& asset)
	{
		if (asset == nullptr) return;

		const auto path = asset->GetPath();
		const auto uuid = asset->GetUUID();

		try
		{
			if (exists(path))
			{
				remove_all(path);
			}

			const auto metaPath = Path(path.string().append(".meta"));
			if (exists(metaPath))
			{
				std::filesystem::remove(metaPath);
			}
		}
		catch (std::exception& e)
		{
			Log::CoreError("Failed to delete asset '{}'. Error: {}", asset->GetName(), e.what());
			return;
		}

		m_AssetsByPath.erase(path);
		m_Assets.erase(uuid);

		DeleteAllAssetsInDirectory(path);
	}

	bool AssetDatabase::AssetExists(const Path& path)
	{
		return m_AssetsByPath.count(path);
	}

	void AssetDatabase::ReimportAllAssetsInDirectory(const Path& directory)
	{
		if (!is_directory(directory)) return;

		for (std::filesystem::recursive_directory_iterator i(directory), end; i != end; ++i)
		{
			if (is_directory(i->path())) continue;

			ImportAsset(*i, true);
		}
	}

	void AssetDatabase::DeleteAllAssetsInDirectory(const Path& directory)
	{
		if (!is_directory(directory)) return;

		for (std::filesystem::recursive_directory_iterator i(directory), end; i != end; ++i)
		{
			const Path& path = i->path();

			if (is_directory(path)) continue;

			if (const auto asset = GetAsset(path))
				m_Assets.erase(asset->GetUUID());

			m_AssetsByPath.erase(path);
		}
	}

	void AssetDatabase::CreateMeta(const Path& path, const UUID uuid, const YAML::Node* extraInfo)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "UUID" << YAML::Value << uuid;
		if (extraInfo) out << *extraInfo;
		out << YAML::EndMap;

		auto fout = std::ofstream{ path };
		fout << out.c_str();
	}
}
