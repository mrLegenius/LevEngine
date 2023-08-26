#pragma once
#include "Assets/Asset.h"
#include "Assets/AssetDatabase.h"
#include "Kernel/Color.h"
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

		static void DrawIntControl(const std::string& label, const std::function<int()>& getter, const std::function<void(int)>& setter,
			int speed = 1, int min = 0, int max = 0);

		static void DrawColor3Control(const std::string& label, const std::function<Color()>& getter,
		                              const std::function<void(Color)>& setter);
		static void DrawTexture2D(Ref<Texture>& texture, Vector2 size = { 100, 100 });
		static void DrawTexture2D(const std::string& label, Ref<TextureAsset>* assetPtr);
		static void DrawTexture2D(const std::function<const Ref<Texture>&()>& getter,
		                          const std::function<void(Ref<Texture>)>& setter, Vector2 size = { 32, 32 });

		static void DrawCheckBox(const char* label, const std::function<bool()>& getter, const std::function<void(bool)>& setter);

		template<class T>
		static void DrawAsset(const std::string& label, Ref<T>* assetPtr)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			const auto& asset = *assetPtr;

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
					*assetPtr = nullptr;

				ImGui::EndPopup();
			}

			if (!ImGui::BeginDragDropTarget()) return;

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayload))
			{
				const auto path = static_cast<const wchar_t*>(payload->Data);
				const std::filesystem::path assetPath = AssetDatabase::AssetsRoot / path;

				if (const auto& newAsset = AssetDatabase::GetAsset<T>(assetPath))
					*assetPtr = newAsset;
			}
			ImGui::EndDragDropTarget();
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
