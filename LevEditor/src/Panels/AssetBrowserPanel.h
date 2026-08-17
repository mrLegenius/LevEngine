#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class AssetBrowserPanel : public Panel
    {
    public:
        AssetBrowserPanel();

        //<--- Navigates the browser to the asset, highlights it and focuses the panel.
        //<--- Deferred, so it can be requested from anywhere during the frame
        static void RevealAsset(const Ref<Asset>& asset);

        //Drops the deferred request, so no asset is kept alive past the editor shutdown
        static void Shutdown() { s_AssetToReveal = nullptr; }

    protected:
        String GetName() override { return "Asset Browser"; }
        void DrawContent() override;

        void DrawFileTree();
        void DrawDirectoryNode(const Path& path, bool isOpenByDefault);
        void DrawAssets();

    private:
        //<--- One entry of the browsed directory. Cached, because the filesystem iteration and the
        //<--- filename string allocations used to be repeated for every entry on every frame ---<<
        struct BrowserEntry
        {
            Path FilePath;
            String FileName;
            String Stem;
            bool IsDirectory;
        };

        template <class AssetType, class ... Args>
        void DrawCreateMenu(const String& label, const String& defaultName, Args ... args);

        //<--- Decoding an image and uploading it to the GPU costs milliseconds, so a folder of
        //<--- textures fills in over the next few frames instead of stalling the frame that opened it ---<<
        static constexpr uint32_t k_ThumbnailsPerFrame = 1;

        //<--- The listing is rebuilt on this interval as well, to pick up changes made outside the editor ---<<
        static constexpr float k_EntriesRefreshInterval = 1.0f;

        [[nodiscard]] Ref<Texture> ResolveIcon(const Ref<Asset>& asset, const BrowserEntry& entry, bool isVisible);

        void RefreshEntries();
        void InvalidateEntries() { m_EntriesDirty = true; }

        inline static Ref<Asset> s_AssetToReveal{};

        Path m_CurrentDirectory;
        Ref<Asset> m_RenamingAsset{};
        Ref<Asset> m_AssetToDelete{};
        Ref<Asset> m_HighlightedAsset{};
        bool m_ScrollToHighlighted = false;

        Vector<BrowserEntry> m_Entries;
        Path m_CachedDirectory;
        float m_NextEntriesRefresh = 0;
        bool m_EntriesDirty = true;

        uint32_t m_ThumbnailsLoadedThisFrame = 0;
    };
}
