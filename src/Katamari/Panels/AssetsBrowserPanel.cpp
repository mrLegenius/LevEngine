#include "pch.h"
#include "AssetsBrowserPanel.h"

#include <imgui.h>

#include "TextureLibrary.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{


    AssetsBrowserPanel::AssetsBrowserPanel() : m_CurrentDirectory(GUIUtils::AssetsPath)
    {
        m_DirectoryIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\DirectoryIcon.png");
        m_FileIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\FileIcon.png");
    }

    void AssetsBrowserPanel::DrawContent()
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
            else if (fileExtension == ".png"
                || fileExtension == ".jpg"
                || fileExtension == ".tga")
                icon = TextureLibrary::GetTexture(path.string());
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
