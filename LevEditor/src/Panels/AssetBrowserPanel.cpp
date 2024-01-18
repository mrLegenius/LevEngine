#include "pch.h"
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
    bool IsSubPath(const Path& base, const Path& destination)
    {
        const Path relative = std::filesystem::relative(destination, base);
        // Size check for a "." result.
        // If the path starts with "..", it's not a subdirectory.
        const std::string string = relative.string();
        return string.size() == 1 || string[0] != '.' && string[1] != '.';
    }

    AssetBrowserPanel::AssetBrowserPanel()
        : m_CurrentDirectory(AssetDatabase::GetAssetsPath())
    {
    }

    void AssetBrowserPanel::DrawContent()
    {
        {
            GUI::ScopedVariable rounding{ImGuiStyleVar_FrameRounding, 0.0f};
            GUI::ScopedVariable borderSize{ImGuiStyleVar_FrameBorderSize, 0.0f};

            ImGui::BeginChild("FileTree", ImVec2{64, 0},
                ImGuiChildFlags_ResizeX | ImGuiChildFlags_Border | ImGuiChildFlags_FrameStyle);

            DrawFileTree();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 10);
            ImGui::EndChild();
        }


        ImGui::SameLine();
        ImGui::BeginChild("Assets");
        DrawAssets();
        ImGui::EndChild();
    }

    void AssetBrowserPanel::DrawFileTree()
    {
        DrawDirectoryNode(AssetDatabase::GetAssetsPath(), true);
    }

    void AssetBrowserPanel::DrawDirectoryNode(const Path& path, const bool isOpenByDefault)
    {
        if (!is_directory(path)) return;

        auto flags =
                        (m_CurrentDirectory == path ? ImGuiTreeNodeFlags_Selected : 0)
                        | (isOpenByDefault ? ImGuiTreeNodeFlags_DefaultOpen : 0)
                        | ImGuiTreeNodeFlags_OpenOnArrow
                        | ImGuiTreeNodeFlags_SpanAvailWidth;

        bool empty = true;
        for (auto &p : std::filesystem::directory_iterator(path))
        {
            if (p.is_directory())
            {
                empty = false;
                break;
            }
        }

        if (empty)
            flags |= ImGuiTreeNodeFlags_Leaf;

        const String stemString = ToString(path.stem());

        if (m_CurrentDirectory != path && IsSubPath(path, m_CurrentDirectory))
            ImGui::SetNextItemOpen(true);

        const bool opened = ImGui::TreeNodeEx(stemString.c_str(), flags);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            m_CurrentDirectory = path;
        }

        if (opened)
        {
            for (const auto& childEntry : std::filesystem::directory_iterator(path))
            {
                DrawDirectoryNode(childEntry, false);
            }
        }

        if (opened)
            ImGui::TreePop();
    }

    void AssetBrowserPanel::DrawAssets()
    {
        if (m_CurrentDirectory != AssetDatabase::GetAssetsPath())
        {
            if (ImGui::Button("<"))
	            m_CurrentDirectory = m_CurrentDirectory.parent_path();

            ImGui::SameLine();
        }

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;

        const auto relativePath = relative(m_CurrentDirectory, Project::GetRoot());

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
            asset = AssetDatabase::GetAsset(path);

            GUI::ScopedID id(filenameString);
            const Ref<Texture> icon = directoryEntry.is_directory()
                                          ? Icons::Directory()
                                          : asset
                                            ? asset->GetIcon()
                                            : Icons::File();

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

                    if (ImGui::MenuItem("Rename"))
                        m_RenamingAsset = asset;

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


            if (m_RenamingAsset && asset == m_RenamingAsset)
            {
                EditorGUI::DrawTextInputField("##Renaming", stemString, [&asset, this](const String& newValue)
                {
                    if (newValue.empty()) return;

                    AssetDatabase::RenameAsset(asset, newValue);
                    m_RenamingAsset = nullptr;
                });
            }
            else
            {
                ImGui::Text(stemString.c_str());

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    m_RenamingAsset = asset;
                }
            }

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
