#include "pch.h"
#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "TextureLibrary.h"
#include "Assets/AssetSelection.h"
#include "Assets/MaterialAsset.h"
#include "GUI/GUIUtils.h"
#include "Katamari/Selection.h"

namespace LevEngine::Editor
{
    AssetBrowserPanel::AssetBrowserPanel() : m_CurrentDirectory(GUIUtils::AssetsPath)
    {
        m_DirectoryIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\DirectoryIcon.png");
        m_FileIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\FileIcon.png");
        m_MeshIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MeshIcon.png");
        m_MaterialIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MaterialIcon.png");
    }

    void AssetBrowserPanel::DrawContent()
    {
        if (m_CurrentDirectory != GUIUtils::AssetsPath)
        {
            if (ImGui::Button("<-"))
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        ImGui::SameLine();
        ImGui::Text(m_CurrentDirectory.string().c_str());

        static float padding = 16.0f;
        static float thumbnailSize = 128.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            std::string filenameString = path.filename().string();
            std::string fileExtension = path.extension().string();

            ImGui::PushID(filenameString.c_str());
            Ref<Texture> icon = nullptr;
            if (directoryEntry.is_directory())
            {
                icon = m_DirectoryIcon;
            }
            else if (GUIUtils::IsAssetTexture(path))
                icon = TextureLibrary::GetTexture(path.string());
            else if (GUIUtils::IsAssetMesh(path))
                icon = m_MeshIcon;
            else if (GUIUtils::IsAssetMaterial(path))
                icon = m_MaterialIcon;
        	else
                icon = m_FileIcon;

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton(icon->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

            if (ImGui::BeginDragDropSource())
            {
                auto relativePath = std::filesystem::relative(path, GUIUtils::AssetsPath);
                const wchar_t* itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload(GUIUtils::AssetPayload, itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }

            if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("Create Asset");
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                if (!directoryEntry.is_directory())
                {
	                if (GUIUtils::IsAssetMaterial(path))
	                {
                        Ref<MaterialAsset> asset = CreateRef<MaterialAsset>(path);
                        asset->Deserialize();
                        Selection::Select(CreateRef<AssetSelection>(asset));
	                }
                }
            }

            if (ImGui::BeginPopup("Create Asset"))
            {
                if (ImGui::Button("Material"))
                {
                    MaterialAsset asset{ m_CurrentDirectory / "Material.mat" };
                    asset.Serialize();
                }

                ImGui::EndPopup();
            }


            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();

            }
            ImGui::TextWrapped(filenameString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);

        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
        ImGui::SliderFloat("Padding", &padding, 0, 32);

        // TODO: status bar
    }
}
