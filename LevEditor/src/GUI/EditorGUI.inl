#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "EntitySelection.h"
#include "Selection.h"
#include "Assets/Asset.h"

namespace LevEngine::Editor
{
	template<class T>
	bool EditorGUI::DrawAsset(const String& label, Ref<T>& assetPtr)
	{
		static_assert(eastl::is_base_of_v<Asset, T>, "T must derive from Asset");

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
	bool EditorGUI::DrawSelectableComponentList(const String& label, Vector<Entity>& entities, int& itemSelectedIdx)
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
				if (EditorGUI::DrawSelectableComponent<T>(componentTypeName, entity, wasSelected, isSelected, idx))
					changed = true;

				if (isSelected)
					itemSelectedIdx = idx;
			}
		
			ImGui::EndListBox();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payloadEntity = ImGui::AcceptDragDropPayload(EntityPayload))
			{
				entities.emplace_back(*static_cast<Entity*>(payloadEntity->Data));
				itemSelectedIdx = static_cast<int>(entities.size() - 1);
				changed = true;
			}
			else if (const ImGuiPayload* payloadFromComponentDrawer = ImGui::AcceptDragDropPayload(typeName))
			{
				entities.emplace_back(*static_cast<Entity*>(payloadFromComponentDrawer->Data));
				itemSelectedIdx = static_cast<int>(entities.size() - 1);
				changed = true;
			}
		
			ImGui::EndDragDropTarget();
		}

		ImGui::AlignTextToFramePadding();
		if (ImGui::Button("+"))
		{
			entities.emplace_back(Entity());
			itemSelectedIdx = static_cast<int>(entities.size() - 1);
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
	bool EditorGUI::DrawSelectableComponent(const String& componentTypeName, Entity& entity, bool wasSelected,
		bool& isSelected, int selectableIdx)
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
	bool EditorGUI::DrawSelectableAssetSet(const String& label, Set<Ref<T>>& assets, Weak<T>& selectedAsset)
	{
		static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");
		bool changed = false;

		auto selectedAssetRef = selectedAsset.lock();
	
		if (ImGui::BeginListBox(label.c_str()))
		{
			for (const auto& asset : assets)
			{
				bool isSelected = false;
				const bool wasSelected = selectedAssetRef == asset;
				
				if (EditorGUI::DrawSelectableAsset<T>(asset, wasSelected, isSelected))
					changed = true;

				if (isSelected)
					selectedAsset = asset;
			}
		
			ImGui::EndListBox();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payloadAsset = ImGui::AcceptDragDropPayload(AssetPayload))
			{
				const Path assetPath = static_cast<const wchar_t*>(payloadAsset->Data);

				if (const auto asset = AssetDatabase::GetAsset<T>(assetPath))
				{
					changed = assets.find(asset) == assets.end();
					if (changed)
					{
						assets.insert(asset);
					}
				}
			}
		
			ImGui::EndDragDropTarget();
		}

		ImGui::AlignTextToFramePadding();

		if (ImGui::Button("-"))
		{
			if (selectedAssetRef)
			{
				assets.erase(selectedAssetRef);
				changed = true;
			}
		}

		return changed;
	}

	template<class T>
	bool EditorGUI::DrawSelectableAsset(const Ref<T>& asset, bool wasSelected,
			bool& isSelected)
	{
		static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");
		auto changed = false;

		ImGui::AlignTextToFramePadding();
		
		const auto label = Format("{}", asset->GetName());
	
		if (ImGui::Selectable(label.c_str(), wasSelected))
		{
			isSelected = true;
		}
		
		if (wasSelected)
			ImGui::SetItemDefaultFocus();
		
		return changed;
	}
	
	template<class T, int N>
	bool EditorGUI::DrawComboBox(const String label, Array<String, N> stringValues, T& value)
	{
		bool changed = false;
		auto currentString = stringValues[static_cast<int>(value)];
		if (ImGui::BeginCombo(label.c_str(), currentString.c_str()))
		{
			for (int i = 0; i < stringValues.size(); i++)
			{
				const bool isSelected = currentString == stringValues[i];
				if (ImGui::Selectable(stringValues[i].c_str(), isSelected))
				{
					currentString = stringValues[i];
					value = static_cast<T>(i);
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return changed;
	}
	
	template<class T, int N>
	bool EditorGUI::DrawComboBox(const String label, Array<String, N> stringValues, const Func<T>& getter, const Action<T>& setter)
	{
		if (T value = getter(); DrawComboBox(label, stringValues, value))
		{
			setter(value);
			return true;
		}

		return false;
	}

	template<int TMaxCharacters>
	void EditorGUI::DrawTextInputField(const String& label, const String& text, const Action<String>& onApply)
	{
		char buffer[TMaxCharacters] = {};
		strcpy_s(buffer, sizeof buffer, text.c_str());

		ImGui::AlignTextToFramePadding();
		if (ImGui::InputText(label.c_str(), buffer, sizeof buffer, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			onApply(buffer);
			return;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			onApply(buffer);
			return;
		}
	}
}
