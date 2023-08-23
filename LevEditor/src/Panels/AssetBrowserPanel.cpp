#include "pch.h"
#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "AssetSelection.h"
#include "Selection.h"

namespace LevEngine::Editor
{
    AssetBrowserPanel::AssetBrowserPanel() : m_CurrentDirectory(GUIUtils::AssetsPath)
    {
        m_DirectoryIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\DirectoryIcon.png");
        m_FileIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\FileIcon.png");
        m_MeshIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MeshIcon.png");
        m_MaterialIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MaterialIcon.png");
        m_SkyboxIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\SkyboxIcon.png");
    }

    void AssetBrowserPanel::DrawContent()
    {
        if (m_CurrentDirectory != GUIUtils::AssetsPath)
        {
            if (ImGui::Button("<"))
	            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }

        ImGui::SameLine();
        ImGui::Text(m_CurrentDirectory.string().c_str());

        static float padding = 16.0f;
        static float thumbnailSize = 128.0f;
        const float cellSize = thumbnailSize + padding;

        const float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            std::string filenameString = path.filename().string();
            std::string stemString = path.stem().string();
            std::string fileExtension = path.extension().string();

            ImGui::PushID(filenameString.c_str());
            Ref<Texture> icon = nullptr;
            if (directoryEntry.is_directory())
	            icon = m_DirectoryIcon;
            else if (GUIUtils::IsAssetTexture(path))
                icon = TextureLibrary::GetTexture(path.string());
            else if (GUIUtils::IsAssetMesh(path))
                icon = m_MeshIcon;
            else if (GUIUtils::IsAssetMaterial(path))
                icon = m_MaterialIcon;
        	else if (GUIUtils::IsAssetSkybox(path))
                icon = m_SkyboxIcon;
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

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                if (!directoryEntry.is_directory())
                {
                    SetAssetSelection<MaterialAsset>(GUIUtils::IsAssetMaterial, path);
                    SetAssetSelection<SkyboxAsset>(GUIUtils::IsAssetSkybox, path);
                }
            }

            if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("Create Asset");
            }

            if (ImGui::BeginPopup("Create Asset"))
            {
                if (ImGui::BeginMenu("Create"))
                {
                    DrawCreateMenu<MaterialAsset>("Material", "Material.mat");
                    DrawCreateMenu<SkyboxAsset>("Skybox", "Skybox.skybox");

                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                    m_CurrentDirectory /= path.filename();

            }
            ImGui::TextWrapped(stemString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);

        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
        ImGui::SliderFloat("Padding", &padding, 0, 32);

        // TODO: status bar
    }

    template <typename AssetType>
    void AssetBrowserPanel::DrawCreateMenu(const std::string& label, const std::string& defaultName) const
    {
        static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (ImGui::MenuItem(label.c_str()))
        {
	        const Ref<Asset> asset = CreateRef<AssetType>(m_CurrentDirectory / defaultName);
            asset->Serialize();
            Selection::Select(CreateRef<AssetSelection>(asset));
        }
    }

    template <typename AssetType>
    void AssetBrowserPanel::SetAssetSelection(const std::function<bool(const std::filesystem::path&)>& validator, const std::filesystem::path& path) const
    {
        static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (validator(path))
        {
            Ref<Asset> asset = CreateRef<AssetType>(path);
            asset->Deserialize();
            Selection::Select(CreateRef<AssetSelection>(asset));
        }
    }
}
