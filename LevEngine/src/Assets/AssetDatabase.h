#pragma once
#include "Asset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"
#include "SkyboxAsset.h"

namespace LevEngine
{
	class AssetDatabase
	{
	public:
		inline static const std::filesystem::path AssetsRoot = "resources";

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
			//if (IsAssetTexture(path))
				//return CreateRef<TextureAsset>(path);
			if (IsAssetMaterial(path))
				return CreateRef<MaterialAsset>(path, uuid);
			else if (IsAssetSkybox(path))
				return CreateRef<SkyboxAsset>(path, uuid);
			else if (IsAssetMesh(path))
				return CreateRef<MeshAsset>(path, uuid);
			else
			{
				Log::CoreTrace("{0} is unsupported format", path.extension());
				return nullptr;
			}
		}

		template<class T>
		[[nodiscard]] static Ref<T> CreateAsset(const std::filesystem::path& path)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			const auto uuid = UUID();

			auto asset = CreateAsset(path, uuid);
			asset->Serialize();

			const std::string metaPath = path.string() + ".meta";

			YAML::Emitter out;

			out << YAML::BeginMap;
			out << YAML::Key << "UUID" << YAML::Value << uuid;
			out << YAML::EndMap;

			auto fout = std::ofstream{ metaPath };
			fout << out.c_str();

			m_Assets.emplace(uuid, asset);
			m_AssetsByPath.emplace(path, asset);

			return std::dynamic_pointer_cast<T>(asset);
		}

		template<class T>
		[[nodiscard]] static const Ref<T>& GetAsset(const UUID uuid)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must be a asset");

			const auto assetIt = m_Assets.find(uuid);
			if (assetIt == m_Assets.end())
			{
				Log::CoreError("Asset with {0} is not found", static_cast<std::uint64_t>(uuid));
				return nullptr;
			}
			const auto asset = assetIt->second;
			auto assetT = std::dynamic_pointer_cast<T>(asset);
			if (!assetT)
			{
				Log::CoreError("Asset with {0} uuid is not {1}", static_cast<std::uint64_t>(uuid), typeid(T).name());
				return nullptr;
			}

			if (!asset->IsDeserialized())
				asset->Deserialize();

			return assetT;
		}

		template<class T>
		[[nodiscard]] static const Ref<T>& GetAsset(const std::filesystem::path& path)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must be a asset");

			const auto assetIt = m_AssetsByPath.find(path);
			if (assetIt == m_AssetsByPath.end())
			{
				Log::CoreError("Asset in {0} is not found", path.string());
				return nullptr;
			}

			const auto asset = assetIt->second;
			auto assetT = std::dynamic_pointer_cast<T>(asset);
			if (!assetT)
			{
				Log::CoreError("Asset in {0} is not {1}", path.string(), typeid(T).name());
				return nullptr;
			}

			if (!asset->IsDeserialized())
				asset->Deserialize();

			return assetT;
		}

	private:
		static inline std::unordered_map<UUID, Ref<Asset>> m_Assets;
		static inline std::unordered_map<std::filesystem::path, Ref<Asset>> m_AssetsByPath;
	};
}
