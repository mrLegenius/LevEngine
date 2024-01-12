#pragma once

namespace LevEngine
{
    struct Color;

    class GUI
    {
    public:
        static Vector2 GetWindowPosition();
        static Vector2 GetWindowSize();
        static Vector2 GetWindowSpacePosition(const Vector2& position);

        static void DrawCircle(const Vector2& relativePosition, float radius, const Color& color, float thickness = 1);

        //TODO: Pass font here
        static void DrawString(const Vector2& relativePosition, const String& text, float size, const Color& color);
    };
}