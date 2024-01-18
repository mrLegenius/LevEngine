#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class AssetBrowserPanel : public Panel
    {
    public:
        AssetBrowserPanel();

    protected:
        String GetName() override { return "Asset Browser"; }
        void DrawContent() override;

        void DrawFileTree();
        void DrawDirectoryNode(const Path& path, bool isOpenByDefault);
        void DrawAssets();

    private:
        template <class AssetType, class ... Args>
        void DrawCreateMenu(const String& label, const String& defaultName, Args ... args) const;

        Path m_CurrentDirectory;
        Ref<Asset> m_RenamingAsset{};
    };
}
