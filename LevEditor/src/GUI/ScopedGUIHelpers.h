#pragma once

namespace LevEngine::Editor::GUI
{
    class ScopedID
    {
    public:
        explicit ScopedID(const String& id) { ImGui::PushID(id.c_str()); }
        explicit ScopedID(const char* id) { ImGui::PushID(id); }
        explicit ScopedID(const void* id) { ImGui::PushID(id); }
        explicit ScopedID(const int id) { ImGui::PushID(id); }
	
        ~ScopedID() { ImGui::PopID(); }
    };
    class ScopedVariable
    {
    public:
        ScopedVariable(const ImGuiStyleVar variable, const float value)
        {
            ImGui::PushStyleVar(variable, value);
        }

        ScopedVariable(const ImGuiStyleVar variable, const Vector2 value)
        {
            ImGui::PushStyleVar(variable, ImVec2{value.x, value.y});
        }

        ~ScopedVariable()
        {
            ImGui::PopStyleVar();
        }
    };
    class ScopedColor
    {
    public:
        ScopedColor(const ImGuiCol colorItem, const Color color)
        {
            ImGui::PushStyleColor(colorItem, ImVec4{color.r, color.g, color.b, color.a});
        }

        ScopedColor(const ImGuiCol colorItem, const ImVec4 color)
        {
            ImGui::PushStyleColor(colorItem, color);
        }

        ~ScopedColor()
        {
            ImGui::PopStyleColor();
        }
    };
    class ScopedFont
    {
    public:
        explicit ScopedFont(ImFont* font)
        {
            ImGui::PushFont(font);
        }

        ~ScopedFont()
        {
            ImGui::PopFont();
        }
    };
    
}
