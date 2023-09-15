#include "pch.h"
#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "AssetSelection.h"
#include "Selection.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    AssetBrowserPanel::AssetBrowserPanel()
        : m_CurrentDirectory(AssetDatabase::GetAssetsPath())
    {
        m_DirectoryIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\DirectoryIcon.png");
        m_FileIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\FileIcon.png");
        m_MeshIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MeshIcon.png");
        m_MaterialIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MaterialIcon.png");
        m_SkyboxIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\SkyboxIcon.png");
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
        
        auto relativePath = relative(m_CurrentDirectory, Project::GetRoot());
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
        int columnCount = (int)(panelWidth / cellSize);
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
            
            ImGui::PushID(filenameString.c_str());
            Ref<Texture> icon = nullptr;
            if (directoryEntry.is_directory())
	            icon = m_DirectoryIcon;
            else if (AssetDatabase::IsAssetTexture(path))
                icon = TextureLibrary::GetTexture(path.string().c_str());
            else if (AssetDatabase::IsAssetMesh(path))
                icon = m_MeshIcon;
            else if (AssetDatabase::IsAssetMaterial(path))
                icon = m_MaterialIcon;
        	else if (AssetDatabase::IsAssetSkybox(path))
                icon = m_SkyboxIcon;
			else
				icon = m_FileIcon;

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::ImageButton(icon->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
            ImGui::PopStyleColor();

            if (directoryEntry.is_directory())
            {
                if (ImGui::BeginDragDropTarget())
                {
	                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIUtils::AssetPayload))
	                {
	                    const auto assetPathPayload = static_cast<const wchar_t*>(payload->Data);
	                    const Path assetPath = AssetDatabase::GetAssetsPath() / assetPathPayload;

                        AssetDatabase::MoveAsset(AssetDatabase::GetAsset(assetPath), path);
	                }
	                ImGui::EndDragDropTarget();
                }
            }

            if (ImGui::BeginDragDropSource())
            {
                auto relativePath = relative(path);
                const wchar_t* itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload(GUIUtils::AssetPayload, itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10, 5 });
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                    AssetDatabase::DeleteAsset(AssetDatabase::GetAsset(path));

                if (ImGui::MenuItem("Reimport"))
                    AssetDatabase::ImportAsset(path);

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10, 5 });
            if (ImGui::BeginPopupContextWindow("Create Asset", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::BeginMenu("Create"))
                {
                    DrawCreateMenu<MaterialAsset>("Material", "Material.mat");
                    DrawCreateMenu<SkyboxAsset>("Skybox", "Skybox.skybox");

                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();
                else
                {
                    if (const auto& selectedAsset = AssetDatabase::GetAsset(path))
                        Selection::Select(CreateRef<AssetSelection>(selectedAsset));
                    else
                        Selection::Deselect();
                }
            }
            ImGui::Text(stemString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);
    }

    template <typename AssetType>
    void AssetBrowserPanel::DrawCreateMenu(const String& label, const String& defaultName) const
    {
        static_assert(eastl::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (ImGui::MenuItem(label.c_str()))
        {
	        const Ref<Asset> asset = AssetDatabase::CreateAsset<AssetType>(m_CurrentDirectory / defaultName.c_str());
            Selection::Select(CreateRef<AssetSelection>(asset));
        }
    }
}
