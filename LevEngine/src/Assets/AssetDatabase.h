#pragma once
#include "Asset.h"

namespace LevEngine
{
	class AssetDatabase
	{
	public:
		inline static const Path AssetsRoot = "resources";
		inline static const Path AssetsCache = "AssetsCache";
		
		static Path GetAssetsPath();
		static Path GetAssetsCachePath();
		static Path GetAssetCachePath(UUID uuid);
		static Path GetRelativePath(const Path& path);
		static void ReimportAsset(const Path& path);

		static void ImportAsset(const Path& path);
		static void ProcessAllAssets();

		static bool IsAssetTexture(const Path& path);

		static bool IsAssetMesh(const Path& path);

		static bool IsAssetMaterial(const Path& path);

		static bool IsAssetPBRMaterial(const Path& path);

		static bool IsAssetSkybox(const Path& path);

		static bool IsAssetPrefab(const Path& path);

		static bool IsAssetScene(const Path& path);

		static bool IsAssetScript(const Path& path);

		static bool IsAssetAudioBank(const Path& path);
		
		static bool IsAssetAnimationClip(const Path& path);
		static bool IsAssetModel(const Path& path);

		[[nodiscard]] static Ref<Asset> CreateAsset(const Path& path, UUID uuid);

		template<class T, class ...Args>
		[[nodiscard]] static Ref<T> CreateNewAsset(const Path& path, Args ...args);

		static void CreateFolder(const Path& path);

		[[nodiscard]] static Ref<Asset> GetAsset(const Path& path, const bool deserialize = true);

		[[nodiscard]] static Ref<Asset> GetAsset(const UUID uuid);

		template<class T>
		[[nodiscard]] static Ref<T> GetAsset(const UUID uuid);

		template<class T>
		[[nodiscard]] static Ref<T> GetAsset(const Path& path);

		template<class T>
		[[nodiscard]] static Vector<Ref<T>> GetAllAssetsOfClass();

		static void RenameAsset(const Ref<Asset>& asset, const String& name);

		static void MoveAsset(const Ref<Asset>& asset, const Path& directory);
		static void DeleteAsset(const Ref<Asset>& asset);
		static bool AssetExists(const Path& path);

	private:
		inline static UnorderedMap<UUID, Ref<Asset>> m_Assets;
		inline static UnorderedMap<Path, Ref<Asset>> m_AssetsByPath;

		static void ReimportAllAssetsInDirectory(const Path& directory);
		static void DeleteAllAssetsInDirectory(const Path& directory);

		template<class T>
		[[nodiscard]] static Ref<T> GetAsset(const Ref<Asset>& asset);

		static void CreateMeta(const Path& path, const UUID uuid, const YAML::Node* extraInfo = nullptr);
	};
}

#include "AssetDatabase.inl"
