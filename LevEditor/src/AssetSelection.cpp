#include "pch.h"
#include "AssetSelection.h"

#include "GUI/EditorGUI.h"
#include "GUI/Drawers/Assets/AssetDrawer.h"

namespace LevEngine
{
    void AssetSelection::DrawProperties()
    {
        if (!m_Asset) return;

        Editor::EditorGUI::DrawTextInputField("##Name", m_Asset->GetName(), [this](const String& newValue)
        {
            AssetDatabase::RenameAsset(m_Asset, newValue);
        });

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 64);
        if (ImGui::Button("Save", ImVec2{ 64, 24 }))
            m_Asset->Serialize();

        Editor::EditorGUI::DrawTextInputField("Address", m_Asset->GetAddress(), [this](const String& newValue)
        {
            m_Asset->SetAddress(newValue);
        });

        ImGui::Separator();

        static auto& assetDrawers = OrderedClassCollection<Editor::IAssetDrawer>::Instance();
			
        for (const auto& assetDrawer : assetDrawers)
        {
            assetDrawer.first->Draw(m_Asset);
        }
    }
}
