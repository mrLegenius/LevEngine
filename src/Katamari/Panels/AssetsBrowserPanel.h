#pragma once
#include "PanelBase.h"
#include "Renderer/Texture.h"

namespace LevEngine::Editor
{
	class AssetsBrowserPanel : public Panel
	{
    public:
        AssetsBrowserPanel();

protected:
    std::string GetName() override { return "Assets Browser"; }
        void DrawContent() override;

    private:
        std::filesystem::path m_CurrentDirectory;

        Ref<Texture> m_DirectoryIcon;
        Ref<Texture> m_FileIcon;
	};
}
