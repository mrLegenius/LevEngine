#pragma once

namespace LevEngine::Editor::PanelTypes
{
    //Panel type names are used as ImGui window ids and are stored in imgui.ini,
    //so changing them resets the saved layout
    inline const String Viewport = "Viewport";
    inline const String Game = "Game";
    inline const String Hierarchy = "Hierarchy";
    inline const String Properties = "Properties";
    inline const String AssetBrowser = "Asset Browser";
    inline const String Console = "Console";
    inline const String Settings = "Settings";
    inline const String Statistics = "Statistics";
    inline const String Scripts = "Scripts";
    inline const String MissingReferences = "Missing References";
}
