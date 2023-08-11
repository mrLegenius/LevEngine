#pragma once
#include "PanelBase.h"
#include "Renderer/Texture.h"

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
        std::filesystem::path m_CurrentDirectory;

        Ref<Texture> m_DirectoryIcon;
        Ref<Texture> m_FileIcon;
	};
}
