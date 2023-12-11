#include "pch.h"
#include "AssetSelection.h"

#include "GUI/Drawers/Assets/AssetDrawer.h"

namespace LevEngine
{
    void AssetSelection::DrawProperties()
    {
        if (!m_Asset) return;

        {
            auto name = m_Asset->GetName();
            char buffer[256] = {};
            strcpy_s(buffer, sizeof buffer, name.c_str());

            ImGui::AlignTextToFramePadding();
            if (ImGui::InputText("##Name", buffer, sizeof buffer, ImGuiInputTextFlags_EnterReturnsTrue))
                AssetDatabase::RenameAsset(m_Asset, buffer);
        }

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 64);
        if (ImGui::Button("Save", ImVec2{ 64, 24 }))
            m_Asset->Serialize();

        {
            const auto address = m_Asset->GetAddress();
            char buffer[256] = {};
            strcpy_s(buffer, sizeof buffer, address.c_str());
            ImGui::AlignTextToFramePadding();
            if (ImGui::InputText("Address", buffer, sizeof buffer, ImGuiInputTextFlags_EnterReturnsTrue))
                m_Asset->SetAddress(buffer);
        }

        ImGui::Separator();

        static auto& assetDrawers = OrderedClassCollection<Editor::IAssetDrawer>::Instance();
			
        for (const auto& assetDrawer : assetDrawers)
        {
            assetDrawer.first->Draw(m_Asset);
        }
    }
}
