﻿#include "pch.h"
#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "AssetSelection.h"
#include "Project.h"
#include "Selection.h"
#include "Assets/MaterialPBRAsset.h"
#include "Assets/ScriptAsset.h"
#include "GUI/EditorGUI.h"
#include "GUI/ScopedGUIHelpers.h"

namespace LevEngine::Editor
{
    AssetBrowserPanel::AssetBrowserPanel()
        : m_CurrentDirectory(AssetDatabase::GetAssetsPath())
    {
    }

    void AssetBrowserPanel::DrawContent()
    {
        if (m_CurrentDirectory != AssetDatabase::GetAssetsPath())
        {
            if (ImGui::Button("<"))
	            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;

        const auto relativePath = relative(m_CurrentDirectory, Project::GetRoot());
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(relativePath.string().c_str());

        ImGui::PushItemWidth(-300);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 400);
        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 128);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(-100);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 175);
        ImGui::SliderFloat("Padding", &padding, 0, 32);
        ImGui::PopItemWidth();

        const float cellSize = thumbnailSize + padding;

        const float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = static_cast<int>(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const Path& path = directoryEntry.path();
            String filenameString = path.filename().string().c_str();
            String stemString = path.stem().string().c_str();
            String fileExtension = path.extension().string().c_str();
            
            if (fileExtension == ".meta") continue;
            
            Ref<Asset> asset = nullptr;
            if (!directoryEntry.is_directory())
                asset = AssetDatabase::GetAsset(path);
            
            GUI::ScopedID id(filenameString);
            const Ref<Texture> icon = directoryEntry.is_directory()
                                          ? Icons::Directory()
                                          : asset->GetIcon();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton(icon->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
            ImGui::PopStyleColor();

            const auto forceSelection =
                Input::IsKeyDown(KeyCode::LeftControl)
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            
            if (ImGui::IsItemHovered() && (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || forceSelection))
            {
                if (directoryEntry.is_directory() && !forceSelection)
                    m_CurrentDirectory /= path.filename();
                else
                {
                    if (const auto& selectedAsset = AssetDatabase::GetAsset(path))
                        Selection::Select(CreateRef<AssetSelection>(selectedAsset));
                    else
                        Selection::Deselect();
                }
            }

            if (directoryEntry.is_directory())
            {
                if (ImGui::BeginDragDropTarget())
                {
	                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EditorGUI::AssetPayload))
	                {
	                    const Path assetPath = static_cast<const wchar_t*>(payload->Data);

                        AssetDatabase::MoveAsset(AssetDatabase::GetAsset(assetPath), path);
	                }
	                ImGui::EndDragDropTarget();
                }
            }

            if (ImGui::BeginDragDropSource())
            {
                const wchar_t* itemPath = path.c_str();
                ImGui::SetDragDropPayload(EditorGUI::AssetPayload, itemPath,
                    (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
                ImGui::EndDragDropSource();
            }

            {
                GUI::ScopedVariable windowPadding {ImGuiStyleVar_WindowPadding, Vector2{ 10, 5 }};
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                        AssetDatabase::DeleteAsset(AssetDatabase::GetAsset(path));

                    if (ImGui::MenuItem("Reimport"))
                        AssetDatabase::ImportAsset(path);

                    ImGui::EndPopup();
                }
            }

            {
                GUI::ScopedVariable windowPadding {ImGuiStyleVar_WindowPadding, Vector2{ 10, 5 }};
                if (ImGui::BeginPopupContextWindow("Create Asset", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
                {
                    if (ImGui::BeginMenu("Create"))
                    {
                        DrawCreateMenu<MaterialPBRAsset>("Material", "Material.pbr");
                        DrawCreateMenu<SkyboxAsset>("Skybox", "Skybox.skybox");
                        if (ImGui::BeginMenu("Script"))
                        {
                            DrawCreateMenu<ScriptAsset>("Script System", "System.lua", ScriptAsset::Type::System);
                            DrawCreateMenu<ScriptAsset>("Script Component", "Component.lua", ScriptAsset::Type::Component);
                            
                            ImGui::EndMenu();
                        }
                        if (ImGui::MenuItem("Folder"))
                            AssetDatabase::CreateFolder(m_CurrentDirectory / "Folder");
                        
                        ImGui::EndMenu();
                    }

                    ImGui::EndPopup();
                }
            }


            ImGui::Text(stemString.c_str());

            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }

    template <typename AssetType, class ...Args>
    void AssetBrowserPanel::DrawCreateMenu(const String& label, const String& defaultName, Args... args) const
    {
        static_assert(eastl::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (ImGui::MenuItem(label.c_str()))
        {
            if (const Ref<Asset> asset = AssetDatabase::CreateNewAsset<AssetType>(m_CurrentDirectory / defaultName.c_str(), eastl::forward<Args>(args)...))
                Selection::Select(CreateRef<AssetSelection>(asset));
        }
    }
}
