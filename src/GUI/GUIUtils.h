﻿#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
	struct GUIUtils
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		inline static const std::filesystem::path AssetsPath = "resources";

		static bool DrawVector3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static void DrawFloatControl(const std::string& label, std::function<float()> getter, std::function<void(float)> setter);
		static void DrawTexture2D(const std::string& label, Ref<Texture>& texture);

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
	};

}
