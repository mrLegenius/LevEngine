#pragma once

namespace LevEngine::Editor
{
	struct EditorGUI
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		static constexpr const char* EntityPayload = "ENTITY";

		static bool DrawVector3Control(const String& label, Vector3& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static void DrawVector3Control(const String& label, const Func<Vector3>& getter, const Action<Vector3>& setter,
			float speed = 1, float min = 0, float max = 0);
		
		static bool DrawVector2Control(const String& label, Vector2& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static void DrawVector2Control(const String& label, const Func<Vector2>& getter, const Action<Vector2>& setter,
			float speed = 1, float min = 0, float max = 0);
		
		static void DrawFloatControl(const String& label, float& value, float speed = 1, float min = 0, float max = 0);
		static void DrawFloatControl(const String& label, const Func<float>& getter, const Action<float>& setter,
			float speed = 1, float min = 0, float max = 0);
		
		static void DrawDoubleControl(const String& label, double& value, double speed, double min = 0, double max = 0);
		static void DrawDoubleControl(const String& label, const Func<double>& getter, const Action<double>& setter,
			double speed, double min = 0, double max = 0);

		//static bool DrawIntControl(const String& label, int& value, int speed, int min, int max);
		static void DrawIntControl(const String& label, const Func<int>& getter, const Action<int>& setter,
			int speed = 1, int min = 0, int max = 0);

		static void DrawColor3Control(const String& label, Color& value);
		static void DrawColor3Control(const String& label, const Func<Color>& getter, const Action<Color>& setter);
		
		static bool DrawTextureAsset(const String& label, Ref<TextureAsset>& assetPtr);
		static bool DrawTextureAsset(Ref<TextureAsset>& assetPtr, Vector2 size);
		
		static void DrawTexture2D(Ref<Texture>& texture, Vector2 size = { 100, 100 });
		static void DrawTexture2D(const Func<Ref<Texture>>& getter,
								  const Action<Ref<Texture>>& setter, Vector2 size = { 32, 32 });

		static bool DrawCheckBox(const char* label, const Func<bool>& getter, const Action<bool>& setter);
		static bool DrawCheckBox(const char* label, bool& value);

		static bool DrawCheckBox3Control(
			const char* mainLabel,
			const char* firstLabel, const Func<bool>& firstGetter, const Action<bool>& firstSetter,
			const char* secondLabel, const Func<bool>& secondGetter, const Action<bool>& secondSetter,
			const char* thirdLabel, const Func<bool>& thirdGetter, const Action<bool>& thirdSetter,
			float labelWidth = 80.0f
		);

		template<class T>
		static bool DrawAsset(const String& label, Ref<T>& assetPtr);

		template<class T>
		static bool DrawSelectableComponentList(const String& label, Vector<Entity>& entities, int& itemSelectedIdx);

		template<class T>
		static bool DrawSelectableComponent(const String& componentTypeName, Entity& entity, bool wasSelected,
			bool& isSelected, int selectableIdx = 0);

		template<class T>
		static bool DrawSelectableAssetSet(const String& label, Set<Ref<T>>& assets, Weak<T>& selectedAsset);

		template<class T>
		static bool DrawSelectableAsset(const Ref<T>& asset, bool wasSelected,
			bool& isSelected);
		
		template<class T, int N>
		static bool DrawComboBox(String label, Array<String, N> stringValues, T& value);

		template<class T, int N>
		static bool DrawComboBox(String label, Array<String, N> stringValues, const Func<T>& getter, const Action<T>& setter);

		template<class T, int N>
		static bool DrawFlagComboBox(String label, Array<String, N> stringValues, T& value);
		
		template<class T, int N>
		static bool DrawFlagComboBox(String label, Array<String, N> stringValues, const Func<T>& getter, const Action<T>& setter);

		template<int TMaxCharacters = 256>
		static void DrawTextInputField(const String& label, const String& text, const Action<String>& onApply);

		template<typename T>
		static void DrawSelectable(const String& label, ImGuiSelectableFlags flags, Vector2 size, T option, T& current);
	};
}

#include "EditorGUI.inl"
