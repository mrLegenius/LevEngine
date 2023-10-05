#pragma once

#include "Assets/AssetDatabase.h"
#include "Math/Color.h"
#include "Renderer/Texture.h"
#include "Scene/Entity.h"
#include "DataTypes/String.h"

namespace LevEngine::Editor
{
	struct EditorGUI
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		static constexpr const char* EntityPayload = "ENTITY";

		static bool DrawVector3Control(const String& label, Vector3& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static bool DrawVector2Control(const String& label, Vector2& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static void DrawFloatControl(const String& label, float& value, float speed = 1, float min = 0, float max = 0);
		static void DrawFloatControl(const String& label, const Func<float>& getter, const Action<float>& setter,
			float speed = 1, float min = 0, float max = 0);
		static void DrawDoubleControl(const String& label, double& value, double speed, double min = 0, double max = 0);
		static void DrawDoubleControl(const String& label, const Func<double>& getter, const Action<double>& setter,
			double speed, double min = 0, double max = 0);

		static void DrawIntControl(const String& label, const Func<int>& getter, const Action<int>& setter,
			int speed = 1, int min = 0, int max = 0);

		static void DrawColor3Control(const String& label, const Func<Color>& getter,
									  const Action<Color>& setter);
		static void DrawTexture2D(Ref<Texture>& texture, Vector2 size = { 100, 100 });
		static bool DrawTextureAsset(const String& label, Ref<TextureAsset>& assetPtr);
		static bool DrawTextureAsset(Ref<TextureAsset>& assetPtr, Vector2 size);
		static void DrawTexture2D(const Func<Ref<Texture>>& getter,
								  const Action<Ref<Texture>>& setter, Vector2 size = { 32, 32 });

		static void DrawCheckBox(const char* label, const Func<bool>& getter, const Action<bool>& setter);

		template<class T>
		static bool DrawAsset(const String& label, Ref<T>& assetPtr);

		template<class T>
		static bool DrawSelectableComponentList(const String& label, Vector<Entity>& entities, int& itemSelectedIdx);

		template<class T>
		static bool DrawSelectableComponent(const String& componentTypeName, Entity& entity, bool wasSelected,
			bool& isSelected, int selectableIdx = 0);

		template<class T>
		static bool DrawComboBox(const char* label, const char* strings[], const size_t stringCount, T* current);
	};
}

#include "EditorGUI.inl"
