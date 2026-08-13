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

    protected:
        String GetName() override { return "Asset Browser"; }
        void DrawContent() override;

        void DrawFileTree();
        void DrawDirectoryNode(const Path& path, bool isOpenByDefault);
        void DrawAssets();

    private:
        template <class AssetType, class ... Args>
        void DrawCreateMenu(const String& label, const String& defaultName, Args ... args) const;

        inline static Ref<Asset> s_AssetToReveal{};

        Path m_CurrentDirectory;
        Ref<Asset> m_RenamingAsset{};
        Ref<Asset> m_AssetToDelete{};
        Ref<Asset> m_HighlightedAsset{};
        bool m_ScrollToHighlighted = false;
    };
}
