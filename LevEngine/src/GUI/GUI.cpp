#include "levpch.h"
#include "GUI.h"

namespace LevEngine
{
    Vector2 GUI::GetWindowPosition()
    {
        const auto currentWindow = ImGui::GetCurrentWindowRead();
        if (currentWindow && !currentWindow->IsFallbackWindow)
        {
            const auto posX = ImGui::GetWindowContentRegionMin().x + ImGui::GetWindowPos().x;
            const auto posY = ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y;
            return {posX, posY};
        }

        const auto pos = ImGui::GetMainViewport()->Pos;
        return {pos.x, pos.y};
    }

    Vector2 GUI::GetWindowSize()
    {
        const auto currentWindow = ImGui::GetCurrentWindowRead();
        if (currentWindow && !currentWindow->IsFallbackWindow)
        {
            const auto size = ImGui::GetContentRegionAvail();
            return {size.x, size.y};
        }

        const auto size = ImGui::GetMainViewport()->Size;
        return {size.x, size.y};
    }

    Vector2 GUI::GetWindowSpacePosition(const Vector2& position)
    {
        const auto result = GetWindowPosition() + GetWindowSize() * position;
        return result;
    }

    void GUI::DrawCircle(const Vector2& relativePosition, const float radius, const Color& color, const float thickness)
    {
        const auto position = GetWindowSpacePosition(relativePosition);
        const auto drawPos = ImVec2{position.x, position.y};

        const ImColor col{color.r, color.g, color.b, color.a};
        
        ImGui::GetForegroundDrawList()->AddCircle(drawPos, radius, col, 0, thickness);
    }

    void GUI::DrawString(const Vector2& relativePosition, const String& text, const float size, const Color& color)
    {
        const auto position = GetWindowSpacePosition(relativePosition);
        const auto drawPos = ImVec2{position.x, position.y};

        const ImColor col{color.r, color.g, color.b, color.a};
        
        ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), size, drawPos, col, text.c_str());
    }
}
