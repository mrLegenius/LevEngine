#pragma once

namespace LevEngine::Editor
{
    class MenuBar;

    //Draws the window caption with ImGui when the platform window has no system title bar.
    //The menu bar is drawn inside it, and everything that is not an item acts as a drag zone
    class TitleBar
    {
    public:
        void Render(const Ref<MenuBar>& menuBar, const String& title);
        [[nodiscard]] float GetHeight() const { return m_Height; }

    private:
        [[nodiscard]] float DrawLogo() const;
        void DrawTitle(const String& title, float dragZoneStart, float dragZoneEnd) const;
        [[nodiscard]] float DrawWindowButtons() const;

        float m_Height{};
    };
}
