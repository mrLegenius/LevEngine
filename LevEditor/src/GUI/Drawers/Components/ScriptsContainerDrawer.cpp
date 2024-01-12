#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"
#include "Scene/Components/ScriptsContainer/ScriptsContainer.h"
#include "Assets/ScriptAsset.h"
#include "Scripting/ScriptingManager.h"

namespace LevEngine::Editor
{
    class ScripsContainerDrawer final : public ComponentDrawer<ScriptsContainer, ScripsContainerDrawer>
    {
    protected:
        String GetLabel() const override { return "ScriptsContainer"; }

        void DrawContent(ScriptsContainer& component) override
        {
			DrawSelectableScriptsComponentSet("Script Components", component.m_ScriptsAssets);
        }
    private:

        void DrawSelectableScriptsComponentSet(const String& label, Set<Ref<ScriptAsset>>& scriptAssets) {

			auto selectedAssetRef = m_SelectedAsset.lock();

			if (ImGui::BeginListBox(label.c_str()))
			{
				for (const auto& asset : scriptAssets)
				{
					bool isSelected = false;
					const bool wasSelected = selectedAssetRef == asset;

					EditorGUI::DrawSelectableAsset<ScriptAsset>(asset, wasSelected, isSelected);

					if (isSelected)
						m_SelectedAsset = asset;
				}

				ImGui::EndListBox();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payloadAsset = ImGui::AcceptDragDropPayload(EditorGUI::AssetPayload))
				{
					const Path assetPath = static_cast<const wchar_t*>(payloadAsset->Data);

					if (const auto asset = AssetDatabase::GetAsset<ScriptAsset>(assetPath))
					{
						if (asset->GetType() == ScriptAsset::Type::Component && scriptAssets.find(asset) == scriptAssets.end())
						{
							scriptAssets.insert(asset);
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
					scriptAssets.erase(selectedAssetRef);
				}
			}
        }

    private:
        Weak<ScriptAsset> m_SelectedAsset;
    };
}
