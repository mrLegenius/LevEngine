﻿#pragma once
#include "Assets/Asset.h"
#include "Kernel/Color.h"
#include "Math/Math.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	struct GUIUtils
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		static constexpr const char* EntityPayload = "ENTITY";

		static bool DrawVector3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static bool DrawVector2Control(const std::string& label, Vector2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static void DrawFloatControl(const std::string& label, float& value, float speed = 1, float min = 0, float max = 0);
		static void DrawFloatControl(const std::string& label, const std::function<float()>& getter, const std::function<void(float)>& setter,
			float speed = 1, float min = 0, float max = 0);
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
	};

}