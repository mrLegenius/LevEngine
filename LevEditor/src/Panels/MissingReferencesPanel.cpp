#include "pch.h"
#include "MissingReferencesPanel.h"

#include "AssetSelection.h"
#include "Project.h"
#include "Selection.h"

namespace LevEngine::Editor
{
    MissingReferencesPanel::MissingReferencesPanel()
    {
        m_DefaultWindowSize = Vector2{ 720, 420 };
    }

    String MissingReferencesPanel::GetName() { return "Missing References"; }

    void MissingReferencesPanel::DrawContent()
    {
        //<--- A scene loaded or a scan run from anywhere else shows up here without a button ---<<
        const auto version = MissingReferences::GetVersion();

        if (m_IsRefreshRequested || version != m_Version)
        {
            m_References = MissingReferences::GetAll();
            m_DanglingAddresses = ResourceManager::GetDanglingAddresses();
            m_IsRefreshRequested = false;
            m_Version = version;
        }

        DrawToolbar();

        ImGui::Separator();

        DrawDanglingAddresses();

        DrawTable();
    }

    void MissingReferencesPanel::DrawToolbar()
    {
        // Scanning reads every YAML asset of the project rather than only the ones that happen to be
        // loaded, which is the only way to see a reference in a scene nobody opened this session.
        if (ImGui::Button("Scan Project"))
        {
            MissingReferences::ScanProject();
            m_IsRefreshRequested = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Refresh"))
            m_IsRefreshRequested = true;

        ImGui::SameLine();

        if (ImGui::Button("Clear"))
        {
            MissingReferences::Clear();
            m_IsRefreshRequested = true;
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(220.0f);

        if (ImGui::InputTextWithHint("##filter", "Filter", m_FilterBuffer, sizeof m_FilterBuffer))
            m_Filter = String(m_FilterBuffer);

        ImGui::SameLine();

        if (m_References.empty())
            ImGui::TextDisabled("No missing references");
        else
            ImGui::Text("%d missing", static_cast<int>(m_References.size()));
    }

    void MissingReferencesPanel::DrawDanglingAddresses()
    {
        if (m_DanglingAddresses.empty()) return;

        if (!ImGui::CollapsingHeader("Dead addresses in ResourcesDatabase", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        ImGui::TextWrapped(
            "These addresses can still be loaded by name from a script, but the asset they point at "
            "is gone. Removing one only drops the entry, the script naming it has to be fixed too.");

        if (ImGui::Button("Remove All"))
        {
            for (const auto& [address, uuid] : m_DanglingAddresses)
                RemoveAddress(address);

            m_IsRefreshRequested = true;
        }

        ImGui::Indent();

        for (const auto& [address, uuid] : m_DanglingAddresses)
        {
            ImGui::PushID(address.c_str());

            if (ImGui::SmallButton("Remove"))
            {
                RemoveAddress(address);
                m_IsRefreshRequested = true;
            }

            ImGui::SameLine();
            ImGui::Text("%s -> %llu", address.c_str(), static_cast<unsigned long long>(uuid));

            ImGui::PopID();
        }

        ImGui::Unindent();
        ImGui::Separator();
    }

    void MissingReferencesPanel::DrawTable()
    {
        constexpr auto flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
            | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp;

        if (!ImGui::BeginTable("MissingReferences", 3, flags)) return;

        ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthStretch, 0.35f);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthStretch, 0.45f);
        ImGui::TableSetupColumn("Missing UUID", ImGuiTableColumnFlags_WidthStretch, 0.20f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        int index = 0;

        for (const auto& reference : m_References)
        {
            if (!Matches(reference)) continue;

            ImGui::TableNextRow();
            ImGui::PushID(index++);

            ImGui::TableSetColumnIndex(0);

            //<--- The whole row selects, so a click anywhere in it opens the asset at fault ---<<
            if (ImGui::Selectable(reference.Source.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                SelectSource(reference.Source);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(reference.Location.empty() ? "-" : reference.Location.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%llu", static_cast<unsigned long long>(reference.Reference));

            if (ImGui::BeginPopupContextItem("row"))
            {
                if (ImGui::MenuItem("Copy UUID"))
                    ImGui::SetClipboardText(Format("{0}", static_cast<uint64_t>(reference.Reference)).c_str());

                if (ImGui::MenuItem("Copy Source"))
                    ImGui::SetClipboardText(reference.Source.c_str());

                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    bool MissingReferencesPanel::Matches(const MissingReference& reference) const
    {
        if (m_Filter.empty()) return true;

        return reference.Source.find(m_Filter) != String::npos
            || reference.Location.find(m_Filter) != String::npos;
    }

    void MissingReferencesPanel::SelectSource(const String& source)
    {
        if (MissingReferences::IsResourcesDatabaseSource(source)) return;

        const auto path = AssetDatabase::GetAssetsPath() / source.c_str();

        const auto asset = AssetDatabase::GetAsset(path);
        if (!asset) return;

        Selection::Select(CreateRef<AssetSelection>(asset));
    }

    void MissingReferencesPanel::RemoveAddress(const String& address)
    {
        if (!ResourceManager::RemoveAddress(address)) return;

        //<--- The database on disk is what the next session reads ---<<
        ResourceManager::Build(Project::GetRoot());
    }
}
