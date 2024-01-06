#pragma once

namespace LevEngine
{
    struct Color;

    class GUI
    {
    public:
        static Vector2 GetWindowPosition();
        static Vector2 GetWindowSize();

        static void DrawCircle(const Vector2& pos, float radius, const Color& color, float thickness = 1);

        //TODO: Pass font here
        static void DrawString(const Vector2& pos, const String& text, float size, const Color& color);
    };
}