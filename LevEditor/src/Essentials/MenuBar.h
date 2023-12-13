#pragma once

namespace LevEngine::Editor
{
    class MenuBar 
    {
    public:
        void AddMenuItem(const String& path, const String& shortcut, const Action<>& callback);
        void Render();
        [[nodiscard]] float GetHeight() const { return m_Height; }
        
    private:
        struct MenuNode
        {
            String Title{};
            String Shortcut{};
            Action<> Callback{};
            Vector<MenuNode> Children{0};

            void Draw() const;
            void InsertItem(String path, const String& shortcut, const Action<>& callback);
            
            inline static const String Delimiter = "/";
        };
        
        MenuNode m_MenuTree{};
        float m_Height{};
    };
}

