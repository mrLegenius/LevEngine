#pragma once
#include "Assets/Asset.h"
#include "Assets/AssetDatabase.h"
#include "DataTypes/Delegates.h"
#include "Kernel/Color.h"
#include "Renderer/Texture.h"

namespace LevEngine
{
	struct GUIUtils
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		static constexpr const char* EntityPayload = "ENTITY";

		static bool DrawVector3Control(const String& label, Vector3& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static bool DrawVector2Control(const String& label, Vector2& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static void DrawFloatControl(const String& label, float& value, float speed = 1, float min = 0, float max = 0);
		static void DrawFloatControl(const String& label, const Func<float>& getter, const Action<float>& setter,
			float speed = 1, float min = 0, float max = 0);

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
		static bool DrawAsset(const String& label, Ref<T>& assetPtr)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			const auto& asset = assetPtr;
			auto changed = false;

			if (!label.empty())
			{
				ImGui::AlignTextToFramePadding();
				ImGui::Text(label.c_str());
				ImGui::SameLine();
			}

			ImGui::AlignTextToFramePadding();
			ImGui::Text(asset ? asset->GetName().c_str() : "None");

			if (asset && ImGui::BeginPopupContextItem("Asset"))
			{
				if (ImGui::MenuItem("Clear"))
				{
					assetPtr = nullptr;
					changed = true;
				}

				ImGui::EndPopup();
			}

			if (!ImGui::BeginDragDropTarget()) return changed;

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayload))
			{
				const Path assetPath = static_cast<const wchar_t*>(payload->Data);

				if (const auto& newAsset = AssetDatabase::GetAsset<T>(assetPath))
				{
					assetPtr = newAsset;
					changed = true;
				}
			}
			ImGui::EndDragDropTarget();

			return changed;
		}

		template<class T>
		static bool DrawComboBox(const char* label, const char* strings[], const size_t stringCount, T* current)
		{
			bool changed = false;
			auto currentString = strings[static_cast<int>(*current)];
			if (ImGui::BeginCombo(label, currentString))
			{
				for (int i = 0; i < stringCount; i++)
				{
					const bool isSelected = currentString == strings[i];
					if (ImGui::Selectable(strings[i], isSelected))
					{
						currentString = strings[i];
						*current = static_cast<T>(i);
						changed = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			return changed;
		}
	};

}
