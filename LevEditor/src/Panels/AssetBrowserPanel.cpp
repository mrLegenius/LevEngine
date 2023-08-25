#include "pch.h"
#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "AssetSelection.h"
#include "Selection.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    AssetBrowserPanel::AssetBrowserPanel() : m_CurrentDirectory(AssetDatabase::AssetsRoot)
    {
        m_DirectoryIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\DirectoryIcon.png");
        m_FileIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\FileIcon.png");
        m_MeshIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MeshIcon.png");
        m_MaterialIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\MaterialIcon.png");
        m_SkyboxIcon = Texture::Create("resources\\Icons\\AssetsBrowser\\SkyboxIcon.png");
    }

    void AssetBrowserPanel::DrawContent()
    {
        if (m_CurrentDirectory != AssetDatabase::AssetsRoot)
        {
            if (ImGui::Button("<"))
	            m_CurrentDirectory = m_CurrentDirectory.parent_path();
        }

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::Text(m_CurrentDirectory.string().c_str());

        ImGui::PushItemWidth(-300);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 400);
        //ImGui::SameLine();
        ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 128);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(-100);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 150);
        //ImGui::SameLine();
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
            const auto& path = directoryEntry.path();
            std::string filenameString = path.filename().string();
            std::string stemString = path.stem().string();
            std::string fileExtension = path.extension().string();

            if (fileExtension == ".meta") continue;

            ImGui::PushID(filenameString.c_str());
            Ref<Texture> icon = nullptr;
            if (directoryEntry.is_directory())
	            icon = m_DirectoryIcon;
            else if (AssetDatabase::IsAssetTexture(path))
                icon = TextureLibrary::GetTexture(path.string());
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

            if (ImGui::BeginDragDropSource())
            {
                auto relativePath = std::filesystem::relative(path, AssetDatabase::AssetsRoot);
                const wchar_t* itemPath = relativePath.c_str();
                ImGui::SetDragDropPayload(GUIUtils::AssetPayload, itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                if (!directoryEntry.is_directory())
                {
                    SetAssetSelection<MaterialAsset>(AssetDatabase::IsAssetMaterial, path);
                    SetAssetSelection<SkyboxAsset>(AssetDatabase::IsAssetSkybox, path);
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

        // TODO: status bar
    }

    template <typename AssetType>
    void AssetBrowserPanel::DrawCreateMenu(const std::string& label, const std::string& defaultName) const
    {
        static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (ImGui::MenuItem(label.c_str()))
        {
	        const Ref<Asset> asset = AssetDatabase::CreateAsset<AssetType>(m_CurrentDirectory / defaultName);
            Selection::Select(CreateRef<AssetSelection>(asset));
        }
    }

    template <typename AssetType>
    void AssetBrowserPanel::SetAssetSelection(const std::function<bool(const std::filesystem::path&)>& validator, const std::filesystem::path& path) const
    {
        static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (validator(path))
        {
            Ref<Asset> asset = AssetDatabase::GetAsset<AssetType>(path);
            Selection::Select(CreateRef<AssetSelection>(asset));
        }
    }
}
