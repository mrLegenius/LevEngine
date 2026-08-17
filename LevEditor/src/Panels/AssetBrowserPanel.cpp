#include "pch.h"
#include "AssetBrowserPanel.h"

#include <imgui.h>

#include "AssetSelection.h"
#include "AssetThumbnailCache.h"
#include "Project.h"
#include "Selection.h"
#include "Assets/MaterialCustomAsset.h"
#include "Assets/MaterialPBRAsset.h"
#include "Assets/ShaderAsset.h"
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

    bool HasSubDirectories(const Path& directory)
    {
        if (!is_directory(directory)) return false;

        for (auto &p : std::filesystem::directory_iterator(directory))
        {
            if (p.is_directory())
                return true;
        }

        return false;
    }

    //<--- Memoized, because the tree used to run a whole directory_iterator for every visible node on
    //<--- every frame only to decide whether the node draws an expand arrow ---<<
    bool HasSubDirectoriesCached(const Path& directory)
    {
        static constexpr float k_CacheLifetime = 1.0f;

        static UnorderedMap<String, bool> cache;
        static float nextClear = 0;

        const float now = Time::GetTimeSinceStartup().GetSeconds();
        if (now >= nextClear)
        {
            cache.clear();
            nextClear = now + k_CacheLifetime;
        }

        const String key = ToString(directory);

        const auto it = cache.find(key);
        if (it != cache.end()) return it->second;

        const bool result = HasSubDirectories(directory);
        cache.emplace(key, result);

        return result;
    }

    AssetBrowserPanel::AssetBrowserPanel()
        : m_CurrentDirectory(AssetDatabase::GetAssetsPath())
    {
        m_DefaultWindowSize = Vector2{ 1030, 390 };
    }

    void AssetBrowserPanel::RevealAsset(const Ref<Asset>& asset)
    {
        if (!asset) return;

        s_AssetToReveal = asset;
    }

    void AssetBrowserPanel::DrawContent()
    {
        LEV_PROFILE_FUNCTION();

        if (s_AssetToReveal)
        {
            const Ref<Asset> asset = s_AssetToReveal;
            s_AssetToReveal = nullptr;

            const Path& path = asset->GetPath();
            m_CurrentDirectory = is_directory(path) ? path : path.parent_path();
            m_HighlightedAsset = asset;
            m_ScrollToHighlighted = true;

            Focus();
        }

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

        if (m_AssetToDelete)
        {
            if (m_CurrentDirectory == m_AssetToDelete->GetPath())
                m_CurrentDirectory = m_AssetToDelete->GetPath().parent_path();

            AssetDatabase::DeleteAsset(m_AssetToDelete);
            m_AssetToDelete = nullptr;
            InvalidateEntries();
        }
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

        if (!HasSubDirectoriesCached(path))
            flags |= ImGuiTreeNodeFlags_Leaf;

        const String stemString = ToString(path.stem());

        if (m_CurrentDirectory != path && IsSubPath(path, m_CurrentDirectory))
            ImGui::SetNextItemOpen(true);

        const bool opened = ImGui::TreeNodeEx(stemString.c_str(), flags);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EditorGUI::AssetPayload))
            {
                const Path assetPath = static_cast<const wchar_t*>(payload->Data);

                AssetDatabase::MoveAsset(AssetDatabase::GetAsset(assetPath, false), path);
                InvalidateEntries();
            }
            ImGui::EndDragDropTarget();
        }

        //<--- Context Popup ---<<
        {
            GUI::ScopedVariable windowPadding {ImGuiStyleVar_WindowPadding, Vector2{ 10, 5 }};
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                   m_AssetToDelete = AssetDatabase::GetAsset(path, false);

                if (ImGui::MenuItem("Reimport"))
                    AssetDatabase::ReimportAsset(path);

                if (ImGui::MenuItem("Open in Explorer"))
                    FileDialogs::OpenFileByExtension(path);

                ImGui::EndPopup();
            }
        }

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

    void AssetBrowserPanel::RefreshEntries()
    {
        LEV_PROFILE_FUNCTION();

        const float now = Time::GetTimeSinceStartup().GetSeconds();

        if (m_CachedDirectory == m_CurrentDirectory && !m_EntriesDirty && now < m_NextEntriesRefresh)
            return;

        m_CachedDirectory = m_CurrentDirectory;
        m_EntriesDirty = false;
        m_NextEntriesRefresh = now + k_EntriesRefreshInterval;

        m_Entries.clear();

        std::error_code errorCode;
        for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory, errorCode))
        {
            const Path& path = directoryEntry.path();

            if (path.extension() == ".meta") continue;

            BrowserEntry entry;
            entry.FilePath = path;
            entry.FileName = ToString(path.filename());
            entry.Stem = ToString(path.stem());
            entry.IsDirectory = directoryEntry.is_directory();

            m_Entries.push_back(eastl::move(entry));
        }
    }

    Ref<Texture> AssetBrowserPanel::ResolveIcon(const Ref<Asset>& asset, const BrowserEntry& entry, const bool isVisible)
    {
        if (!asset) return Icons::File();

        //<--- Every type but a texture has a static icon and never has to touch its file ---<<
        if (!AssetDatabase::IsAssetTexture(entry.FilePath))
            return asset->GetIcon();

        //<--- A small preview of its own, never the asset's texture: previewing by deserializing meant
        //<--- one cell could pull in half a gigabyte that nothing ever released ---<<
        Ref<Texture> thumbnail;
        if (AssetThumbnailCache::TryGet(entry.FilePath, thumbnail))
            return thumbnail ? thumbnail : Icons::File();

        if (!isVisible || m_ThumbnailsLoadedThisFrame >= k_ThumbnailsPerFrame)
            return Icons::File();

        ++m_ThumbnailsLoadedThisFrame;

        thumbnail = AssetThumbnailCache::Load(entry.FilePath);

        return thumbnail ? thumbnail : Icons::File();
    }

    void AssetBrowserPanel::DrawAssets()
    {
        m_ThumbnailsLoadedThisFrame = 0;
        RefreshEntries();

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

        for (const BrowserEntry& entry : m_Entries)
        {
            const Path& path = entry.FilePath;
            const String& filenameString = entry.FileName;
            const String& stemString = entry.Stem;

            //<--- Deserialized lazily: every type but a texture answers GetIcon with a static engine
            //<--- icon, and parsing a folder of models or materials here used to hang the editor ---<<
            Ref<Asset> asset = AssetDatabase::GetAsset(path, false);

            GUI::ScopedID id(filenameString);

            //<--- Tested before the cell is submitted, so a thumbnail below the fold costs nothing ---<<
            const bool isVisible = ImGui::IsRectVisible(ImVec2{ thumbnailSize, thumbnailSize });

            const Ref<Texture> icon = entry.IsDirectory
                                          ? Icons::Directory()
                                          : ResolveIcon(asset, entry, isVisible);

            const bool isHighlighted = asset && asset == m_HighlightedAsset;

            ImGui::PushStyleColor(ImGuiCol_Button, isHighlighted
                ? ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)
                : ImVec4(0, 0, 0, 0));
            ImGui::ImageButton(icon->GetId(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
            ImGui::PopStyleColor();

            if (isHighlighted && m_ScrollToHighlighted)
            {
                m_ScrollToHighlighted = false;
                ImGui::SetScrollHereY(0.5f);
            }

            const auto forceSelection =
                Input::IsKeyDown(KeyCode::LeftControl)
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

            if (ImGui::IsItemHovered() && (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || forceSelection))
            {
                if (entry.IsDirectory && !forceSelection)
                    m_CurrentDirectory /= path.filename();
                else
                {
                    //<--- Deserializing on purpose: the inspector reads the asset's contents and its
                    //<--- Save button calls Serialize(), which would write defaults over the file ---<<
                    if (const auto& selectedAsset = AssetDatabase::GetAsset(path))
                        Selection::Select(CreateRef<AssetSelection>(selectedAsset));
                    else
                        Selection::Deselect();
                }
            }

            if (entry.IsDirectory)
            {
                if (ImGui::BeginDragDropTarget())
                {
	                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EditorGUI::AssetPayload))
	                {
	                    const Path assetPath = static_cast<const wchar_t*>(payload->Data);

                        AssetDatabase::MoveAsset(AssetDatabase::GetAsset(assetPath, false), path);
                        InvalidateEntries();
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
                    {
                        AssetDatabase::DeleteAsset(asset);
                        InvalidateEntries();
                    }

                    if (ImGui::MenuItem("Reimport"))
                        AssetDatabase::ReimportAsset(path);

                    if (ImGui::MenuItem("Rename"))
                        m_RenamingAsset = asset;

                    if (entry.IsDirectory && ImGui::MenuItem("Open in Explorer"))
                        FileDialogs::OpenFileByExtension(path);

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
                        DrawCreateMenu<MaterialCustomAsset>("Shader Material", "Material.material");
                        DrawCreateMenu<ShaderAsset>("Shader", "Shader.hlsl", ShaderAsset::CreateFrom::Template);
                        DrawCreateMenu<SkyboxAsset>("Skybox", "Skybox.skybox");
                        if (ImGui::BeginMenu("Script"))
                        {
                            DrawCreateMenu<ScriptAsset>("Script System", "System.lua", ScriptAsset::Type::System);
                            DrawCreateMenu<ScriptAsset>("Script Component", "Component.lua", ScriptAsset::Type::Component);

                            ImGui::EndMenu();
                        }
                        if (ImGui::MenuItem("Folder"))
                        {
                            m_RenamingAsset = AssetDatabase::CreateFolder(m_CurrentDirectory / "Folder");
                            InvalidateEntries();
                        }

                        ImGui::EndMenu();
                    }

                    ImGui::EndPopup();
                }
            }


            if (m_RenamingAsset && asset == m_RenamingAsset)
            {
                EditorGUI::DrawTextInputField("##Renaming", stemString, [&asset, this](const String& newValue)
                {
                    m_RenamingAsset = nullptr;
                    if (newValue.empty()) return;

                    AssetDatabase::RenameAsset(asset, newValue);
                    InvalidateEntries();
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
    void AssetBrowserPanel::DrawCreateMenu(const String& label, const String& defaultName, Args... args)
    {
        static_assert(eastl::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        if (ImGui::MenuItem(label.c_str()))
        {
            if (Ref<Asset> asset = AssetDatabase::CreateNewAsset<AssetType>(m_CurrentDirectory / defaultName.c_str(), eastl::forward<Args>(args)...))
            {
                Selection::Select(CreateRef<AssetSelection>(asset));
            }

            InvalidateEntries();
        }
    }
}
