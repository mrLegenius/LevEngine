#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class AssetBrowserPanel : public Panel
    {
    public:
        AssetBrowserPanel();

    protected:
        std::string GetName() override { return "Asset Browser"; }
        void DrawContent() override;

    private:
        template<typename Asset>
        void DrawCreateMenu(const std::string& label, const std::string& defaultName) const;

        std::filesystem::path m_CurrentDirectory;

        Ref<Texture> m_DirectoryIcon;
        Ref<Texture> m_FileIcon;
        Ref<Texture> m_MeshIcon;
        Ref<Texture> m_MaterialIcon;
        Ref<Texture> m_SkyboxIcon;
    };
}
