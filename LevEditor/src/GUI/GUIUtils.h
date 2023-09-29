#pragma once
#include "EntitySelection.h"
#include "Selection.h"
#include "Assets/Asset.h"
#include "Assets/AssetDatabase.h"
#include "Kernel/Color.h"
#include "Renderer/Texture.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/TypeParseTraits.h"
#include "DataTypes/String.h"

namespace LevEngine::Editor
{
	struct GUIUtils
	{
		static constexpr const char* AssetPayload = "ASSETS_BROWSER_ITEM";
		static constexpr const char* EntityPayload = "ENTITY";
		static constexpr const char* ComponentPayload = "COMPONENT";

		static bool DrawVector3Control(const String& label, Vector3& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static bool DrawVector2Control(const String& label, Vector2& values, float resetValue = 0.0f, float labelWidth = 100.0f);
		static void DrawFloatControl(const String& label, float& value, float speed = 1, float min = 0, float max = 0);
		static void DrawFloatControl(const String& label, const std::function<float()>& getter, const std::function<void(float)>& setter,
			float speed = 1, float min = 0, float max = 0);

		static void DrawIntControl(const String& label, const std::function<int()>& getter, const std::function<void(int)>& setter,
			int speed = 1, int min = 0, int max = 0);

		static void DrawColor3Control(const String& label, const std::function<Color()>& getter,
		                              const std::function<void(Color)>& setter);
		static void DrawTexture2D(Ref<Texture>& texture, Vector2 size = { 100, 100 });
		static bool DrawTextureAsset(const String& label, Ref<TextureAsset>* assetPtr);
		static bool DrawTextureAsset(Ref<TextureAsset>* assetPtr, Vector2 size);
		static void DrawTexture2D(const std::function<const Ref<Texture>&()>& getter,
		                          const std::function<void(Ref<Texture>)>& setter, Vector2 size = { 32, 32 });

		static void DrawCheckBox(const char* label, const std::function<bool()>& getter, const std::function<void(bool)>& setter);

		template<class T>
		static bool DrawAsset(const String& label, Ref<T>* assetPtr)
		{
			static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

			const auto& asset = *assetPtr;
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
					*assetPtr = nullptr;
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
					*assetPtr = newAsset;
					changed = true;
				}
			}
			ImGui::EndDragDropTarget();

			return changed;
		}

		template<class T>
		static bool DrawSelectableComponentList(const String& label, Vector<Entity>& entities, int& itemSelectedIdx)
		{
			bool changed = false;

			const char* typeName = TypeParseTraits<T>::name;
			
			if (ImGui::BeginListBox(label.c_str()))
			{
				const int count = static_cast<int>(entities.size());
				for (int idx = 0; idx < count; idx++)
				{
					const bool wasSelected = (itemSelectedIdx == idx);
					bool isSelected = false;
					Entity& entity = entities[idx];

					auto componentTypeName = Format("({})", typeName);
					if (GUIUtils::DrawSelectableComponent<T>(componentTypeName, entity, wasSelected, isSelected, idx))
					{
						changed = true;
					}

					if (isSelected)
					{
						itemSelectedIdx = idx;
					}
				}
				
				ImGui::EndListBox();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payloadEntity = ImGui::AcceptDragDropPayload(EntityPayload))
				{
					entities.emplace_back(*static_cast<Entity*>(payloadEntity->Data));
					itemSelectedIdx = entities.size() - 1;
					changed = true;
				}
				else if (const ImGuiPayload* payloadFromComponentDrawer = ImGui::AcceptDragDropPayload(typeName))
				{
					entities.emplace_back(*static_cast<Entity*>(payloadFromComponentDrawer->Data));
					itemSelectedIdx = entities.size() - 1;
					changed = true;
				}
				
				ImGui::EndDragDropTarget();
			}

			ImGui::AlignTextToFramePadding();
			if (ImGui::Button("+"))
			{
				entities.emplace_back(Entity());
				itemSelectedIdx = entities.size() - 1;
				changed = true;
			}
			ImGui::SameLine();

			if (ImGui::Button("-"))
			{
				if (itemSelectedIdx > 0 && itemSelectedIdx < entities.size())
				{
					entities.erase_first(entities[itemSelectedIdx]);
					--itemSelectedIdx;
					
					changed = true;
				}
				else if (entities.size() > 0)
				{
					entities.pop_back();
					changed = true;
				}
			}

			return changed;
		}

		template<class T>
		static bool DrawSelectableComponent(const String& componentTypeName, Entity& entity, bool wasSelected,
			bool& isSelected, int selectableIdx = 0)
		{
			const bool entityHasComponent = entity && entity.HasComponent<T>();
			
			auto changed = false;

			ImGui::AlignTextToFramePadding();
			const char* entityName = entityHasComponent ? entity.GetName().c_str() : "None";
			
			const auto label = Format("{} {}##{}", entityName, componentTypeName, selectableIdx);
			
			if (ImGui::Selectable(label.c_str(), wasSelected, ImGuiSelectableFlags_AllowDoubleClick))
			{
				isSelected = true;
				
				if (ImGui::IsMouseDoubleClicked(0))
				{
					if (entityHasComponent)
					{
						Selection::Select(CreateRef<EntitySelection>(entity));
					}
				}
			}
			
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (wasSelected)
				ImGui::SetItemDefaultFocus();

			if (entityHasComponent && ImGui::BeginPopupContextItem("Component"))
			{
				if (ImGui::MenuItem("Clear"))
				{
					entity = Entity();
					changed = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payloadEntity = ImGui::AcceptDragDropPayload(EntityPayload))
				{
					entity = *static_cast<Entity*>(payloadEntity->Data);
				}
				else if (const ImGuiPayload* payloadFromComponentDrawer = ImGui::AcceptDragDropPayload(TypeParseTraits<T>::name))
				{
					entity = *static_cast<Entity*>(payloadFromComponentDrawer->Data);
				}
				
				ImGui::EndDragDropTarget();
			}
			
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
