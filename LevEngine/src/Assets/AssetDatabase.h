﻿#pragma once
#include "Asset.h"
#include "DefaultAsset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "SkyboxAsset.h"
#include "TextureAsset.h"
#include "DataTypes/UnorderedMap.h"

namespace LevEngine
{
	class AssetDatabase
	{
	public:
		inline static const std::filesystem::path AssetsRoot = "resources";

		static void ImportAsset(const std::filesystem::path& path);
		static void ProcessAllAssets();

		static bool IsAssetTexture(const std::filesystem::path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".tga";
		}

		static bool IsAssetMesh(const std::filesystem::path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".obj";
		}

		static bool IsAssetMaterial(const std::filesystem::path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".mat";
		}

		static bool IsAssetSkybox(const std::filesystem::path& path)
		{
			const auto extension = path.extension().string();

			return extension == ".skybox";
		}

		[[nodiscard]] static Ref<Asset> CreateAsset(const std::filesystem::path& path, UUID uuid)
		{
			if (IsAssetTexture(path))
				return CreateRef<TextureAsset>(path, uuid);

			if (IsAssetMaterial(path))
				return CreateRef<MaterialAsset>(path, uuid);

			if (IsAssetSkybox(path))
				return CreateRef<SkyboxAsset>(path, uuid);

			if (IsAssetMesh(path))
				return CreateRef<MeshAsset>(path, uuid);

			return CreateRef<DefaultAsset>(path, uuid);
		}

		template<class T>
		[[nodiscard]] static Ref<T> CreateAsset(const std::filesystem::path& path)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			const auto uuid = UUID();

			auto asset = CreateAsset(path, uuid);
			asset->Serialize();

			String metaPath = path.string().c_str();
			metaPath += ".meta";

			CreateMeta(metaPath.c_str(), uuid);

			m_Assets.emplace(uuid, asset);
			m_AssetsByPath.emplace(path, asset);

			return CastRef<T>(asset);
		}

		[[nodiscard]] static const Ref<Asset>& GetAsset(const std::filesystem::path& path)
		{
			const auto assetIt = m_AssetsByPath.find(path);
			if (assetIt == m_AssetsByPath.end())
			{
				Log::CoreWarning("Asset in {0} is not found", path.string());
				return nullptr;
			}

			const auto& asset = assetIt->second;
			if (!asset->IsDeserialized())
				asset->Deserialize();

			return asset;
		}

		[[nodiscard]] static const Ref<Asset>& GetAsset(const UUID uuid)
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
		[[nodiscard]] static const Ref<T>& GetAsset(const UUID uuid)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must be a asset");

			return GetAsset<T>(GetAsset(uuid));
		}

		template<class T>
		[[nodiscard]] static const Ref<T>& GetAsset(const std::filesystem::path& path)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must be a asset");

			return GetAsset<T>(GetAsset(path));
		}

		static void RenameAsset(const Ref<Asset>& asset, const String& name);
		static void MoveAsset(const Ref<Asset>& asset, const std::filesystem::path& directory);
		static void DeleteAsset(const Ref<Asset>& asset);

	private:
		static inline UnorderedMap<UUID, Ref<Asset>> m_Assets;
		static inline UnorderedMap<std::filesystem::path, Ref<Asset>> m_AssetsByPath;

		template<class T>
		[[nodiscard]] static const Ref<T>& GetAsset(const Ref<Asset>& asset)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must be a asset");
			if (!asset) return nullptr;

			auto assetT = CastRef<T>(asset);
			if (!assetT)
			{
				Log::CoreWarning("Asset ({0}) in {1} is not {2}", asset->GetUUID(), asset->GetPath(), typeid(T).name());
				return nullptr;
			}

			return assetT;
		}

		static void CreateMeta(const std::filesystem::path& path, const UUID uuid, const YAML::Node* extraInfo = nullptr)
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
