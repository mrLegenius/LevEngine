#pragma once
#include "AnimationAsset.h"
#include "Asset.h"
#include "AudioBankAsset.h"
#include "DefaultAsset.h"
#include "MaterialPBRAsset.h"
#include "MaterialSimpleAsset.h"
#include "MeshAsset.h"
#include "PrefabAsset.h"
#include "Project.h"
#include "SkyboxAsset.h"
#include "TextureAsset.h"

namespace LevEngine
{
	class AssetDatabase
	{
	public:
		inline static const Path AssetsRoot = "resources";
		
		static Path GetAssetsPath()
		{
			if (Project::GetProject())
				return Project::GetRoot() / AssetsRoot;

			return AssetsRoot;
		}
		
		static void ImportAsset(const Path& path);
		static void ProcessAllAssets();

		static bool IsAssetTexture(const Path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga";
		}

		static bool IsAssetMesh(const Path& path)
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

		static bool IsAssetAnimationClip(const Path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".anim";
		}

		static bool IsAssetMaterial(const Path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".mat";
		}

		static bool IsAssetPBRMaterial(const Path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".pbr";
		}

		static bool IsAssetSkybox(const Path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".skybox";
		}

		static bool IsAssetPrefab(const Path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".prefab";
		}

		static bool IsAssetAudioBank(const Path& path)
		{
			const String pathString = ToString(path);
			const size_t index = pathString.find('.');
			if (index == eastl::string::npos)
			{
				return false;
			}

			const auto extension = pathString.right(pathString.length() - index);

			return extension == ".bank";
		}

		template<class T>
		[[nodiscard]] static Ref<T> CreateAsset(const Path& path)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			if (exists(path))
			{
				Log::CoreError("Asset already exists in {}", path);
				return nullptr;
			}
			
			const auto uuid = UUID();

			Ref<Asset> asset = CreateAsset(path, uuid);
			asset->Serialize();

			String metaPath = path.string().c_str();
			metaPath += ".meta";

			CreateMeta(metaPath.c_str(), uuid);

			m_Assets.emplace(uuid, asset);
			m_AssetsByPath.emplace(path, asset);

			return CastRef<T>(asset);
		}
		
		static void CreateFolder(const Path& path)
		{
			if (!exists(path))
				create_directory(path);
		}

		[[nodiscard]] static Ref<Asset> GetAsset(const UUID uuid)
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

		template<class T>
		[[nodiscard]] static Ref<T> GetAsset(const UUID uuid)
		{
			static_assert(eastl::is_base_of_v<Asset, T>, "T must be an asset");

			return CastAsset<T>(GetAsset(uuid));
		}

		[[nodiscard]] static Ref<Asset> GetAsset(const Path& path, const bool deserialize = true)
		{
			const auto assetIt = m_AssetsByPath.find(path);
			if (assetIt == m_AssetsByPath.end())
			{
				Log::CoreWarning("Asset in {0} is not found", path.string());
				return nullptr;
			}
			
			const auto& asset = assetIt->second;
			if (!asset->IsDeserialized() && deserialize)
				asset->Deserialize();

			return asset;
		}

		template<class T>
		[[nodiscard]] static Ref<T> GetAsset(const Path& path, const bool deserialize = true)
		{
			static_assert(eastl::is_base_of_v<Asset, T>, "T must be an asset");

			return CastAsset<T>(GetAsset(path, deserialize));
		}

		static void RenameAsset(const Ref<Asset>& asset, const String& name);
		static void MoveAsset(const Ref<Asset>& asset, const Path& directory);
		static void DeleteAsset(const Ref<Asset>& asset);
		static bool AssetExists(const Path& path);

	private:
		inline static UnorderedMap<UUID, Ref<Asset>> m_Assets;
		inline static UnorderedMap<Path, Ref<Asset>> m_AssetsByPath;

		[[nodiscard]] static Ref<Asset> CreateAsset(const Path& path, UUID uuid)
		{
			if (IsAssetTexture(path))
			{
				return CreateRef<TextureAsset>(path, uuid);
			}

			if (IsAssetMaterial(path))
			{
				return CreateRef<MaterialSimpleAsset>(path, uuid);
			}

			if (IsAssetPBRMaterial(path))
			{
				return CreateRef<MaterialPBRAsset>(path, uuid);
			}

			if (IsAssetSkybox(path))
			{
				return CreateRef<SkyboxAsset>(path, uuid);
			}

			if (IsAssetMesh(path))
			{
				return CreateRef<MeshAsset>(path, uuid);
			}

			if (IsAssetAnimationClip(path))
			{
				return CreateRef<AnimationAsset>(path, uuid);
			}

			if (IsAssetPrefab(path))
			{
				return CreateRef<PrefabAsset>(path, uuid);
			}

			if (IsAssetAudioBank(path))
			{
				return CreateRef<AudioBankAsset>(path, uuid);
			}

			return CreateRef<DefaultAsset>(path, uuid);
		}

		template<class T>
		[[nodiscard]] static Ref<T> CastAsset(const Ref<Asset>& asset)
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

		static void CreateMeta(const Path& path, const UUID uuid, const YAML::Node* extraInfo = nullptr)
		{
			YAML::Emitter out;

			out << YAML::BeginMap;
			out << YAML::Key << "UUID" << YAML::Value << uuid;
			if (extraInfo) out << *extraInfo;
			out << YAML::EndMap;

			auto fout = std::ofstream{ path };
			fout << out.c_str();
		}
	};
}
