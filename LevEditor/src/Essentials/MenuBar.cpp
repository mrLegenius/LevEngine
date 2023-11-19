#include "pch.h"
#include "MenuBar.h"

namespace LevEngine::Editor
{
    void MenuBar::AddMenuItem(const String& path, const String& shortcut, const Action<>& callback)
    {
        m_MenuTree.InsertItem(path, shortcut, callback);
    }

    void MenuBar::Render() const
    {
        if (ImGui::BeginMenuBar())
        {
            m_MenuTree.Draw();
            ImGui::EndMenuBar();
        }
    }

    void MenuBar::MenuNode::Draw() const
    {
        //Leaf -> actual menu item
        if (Children.empty())
        {
            if (ImGui::MenuItem(Title.c_str(), Shortcut.c_str()))
                Callback();

            return;
        }

        //Root
        if (Title.empty())
        {
            for ( auto & n: Children )
                n.Draw();
            
            return;
        }
        
        //Submenu
        if (ImGui::BeginMenu(Title.c_str()))
        {
            for ( auto & n: Children )
                n.Draw();
            
            ImGui::EndMenu();
        }
    }

    void MenuBar::MenuNode::InsertItem(String path, const String& shortcut, const Action<>& callback)
    {
        size_t pos;
       
        if ((pos = path.find(Delimiter)) == std::string::npos)
        {
            //Inserting leaf
            Children.push_back(MenuNode{.Title = path, .Shortcut = shortcut, .Callback = callback});
            return;
        }

        const String token = path.substr(0, pos);
            
        MenuNode* nextNode = eastl::find_if(Children.begin(), Children.end(),
                                            [&token](const MenuNode& node ){ return node.Title == token; });
        
        if (nextNode == Children.end())
        {
            //Didn't find submenu
            Children.push_back(MenuNode{.Title = token});
            nextNode = &Children.back();
        }

        //Moving to next submenu 
        path.erase(0, pos + Delimiter.length());
        nextNode->InsertItem(path, shortcut, callback);
    }
}
