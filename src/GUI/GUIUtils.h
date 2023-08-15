#pragma once
#include "Assets/Asset.h"
#include "Kernel/Color.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	struct GUIUtils
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		inline static const std::filesystem::path AssetsPath = "resources";

		static bool DrawVector3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static bool DrawVector2Control(const std::string& label, Vector2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static void DrawFloatControl(const std::string& label, float& value);
		static void DrawFloatControl(const std::string& label, const std::function<float()>& getter, const std::function<void(float)>& setter);
		static void DrawColor3Control(const std::string& label, const std::function<Color()>& getter,
		                              const std::function<void(Color)>& setter);
		static void DrawTexture2D(Ref<Texture>& texture, Vector2 size = { 100, 100 });
		static void DrawTexture2D(const std::function<const Ref<Texture>&()>& getter,
		                          const std::function<void(Ref<Texture>)>& setter, Vector2 size = { 100, 100 });

		static void DrawAsset(const std::string& label, const std::function<bool(const std::filesystem::path&)>& validation, const std::function<void
		                      (const std::filesystem::path&)>& onDrop);
		static void DrawAsset(const std::string& label, const Ref<Asset>& asset,
		                      const std::function<bool(const std::filesystem::path&)>& validation,
		                      const std::function<void(const std::filesystem::path&)>& onDrop);

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
	};

}
