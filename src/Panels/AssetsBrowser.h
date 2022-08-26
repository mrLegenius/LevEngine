#pragma once

#include "LevEngine.h"

#include <filesystem>

namespace LevEngine
{
    class AssetsBrowser
    {
    public:
        AssetsBrowser();

        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory;

        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
    };
};

