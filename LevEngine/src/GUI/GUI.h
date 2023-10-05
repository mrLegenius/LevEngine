#pragma once

#include "Math/Color.h"
#include "Math/Vector2.h"

namespace LevEngine
{
    class GUI
    {
    public:
        static Vector2 GetWindowPosition();

        static Vector2 GetWindowSize();

        static void DrawCircle(Vector2 pos, float radius, Color color, float thickness = 1);

        //TODO: Pass font here
        static void DrawString(Vector2 pos, const String& text, float size, Color color);
    };
}