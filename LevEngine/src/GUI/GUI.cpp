#include "levpch.h"
#include "GUI.h"

namespace LevEngine
{
    Vector2 GUI::GetWindowPosition()
    {
        const auto pos = ImGui::GetMainViewport()->Pos;
        return {pos.x, pos.y};
    }

    Vector2 GUI::GetWindowSize()
    {
        const auto size = ImGui::GetMainViewport()->Size;
        return {size.x, size.y};
    }

    void GUI::DrawCircle(const Vector2 pos, const float radius, const Color color, const float thickness)
    {
        const auto position = GetWindowPosition() + pos;
        const auto drawPos = ImVec2{position.x, position.y};

        const ImColor col{color.r, color.g, color.b, color.a};
        
        ImGui::GetForegroundDrawList()->AddCircle(drawPos, radius, col, 0, thickness);
    }

    void GUI::DrawString(const Vector2 pos, const String& text, const float size, const Color color)
    {
        const auto position = GetWindowPosition() + pos;
        const auto drawPos = ImVec2{position.x, position.y};

        const ImColor col{color.r, color.g, color.b, color.a};
        
        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), size, drawPos, col, text.c_str());
    }
}
