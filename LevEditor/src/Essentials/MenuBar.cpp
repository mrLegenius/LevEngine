#include "pch.h"
#include "MenuBar.h"

namespace LevEngine::Editor
{
    namespace
    {
        //ImGui only draws menu bars at the fixed position it reserves inside a window, so these
        //are ImGui::BeginMenuBar/EndMenuBar with an explicit rect instead of window->MenuBarRect()
        bool BeginMenuBarInRect(const ImRect& rect)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems) return false;

            LEV_ASSERT(!window->DC.MenuBarAppending);

            ImGui::BeginGroup();
            ImGui::PushID("##menubar");

            ImRect clipRect = rect;
            clipRect.ClipWith(window->OuterRectClipped);
            ImGui::PushClipRect(clipRect.Min, clipRect.Max, false);

            window->DC.CursorPos = window->DC.CursorMaxPos = rect.Min;
            window->DC.LayoutType = ImGuiLayoutType_Horizontal;
            window->DC.IsSameLine = false;
            window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
            window->DC.MenuBarAppending = true;
            ImGui::AlignTextToFramePadding();

            return true;
        }

        void EndMenuBarInRect()
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems) return;

            ImGui::PopClipRect();
            ImGui::PopID();

            //BeginMenuBar backs the layer 0 position up with a group, and the group itself
            //should not turn into an item
            ImGui::GetCurrentContext()->GroupStack.back().EmitItem = false;
            ImGui::EndGroup();

            window->DC.LayoutType = ImGuiLayoutType_Vertical;
            window->DC.IsSameLine = false;
            window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
            window->DC.MenuBarAppending = false;
        }
    }

    void MenuBar::AddMenuItem(const String& path, const String& shortcut, const Action<>& callback)
    {
        m_MenuTree.InsertItem(path, shortcut, callback);
    }

    void MenuBar::Render()
    {
        m_Height = ImGui::GetCurrentWindow()->MenuBarHeight();
        
        if (ImGui::BeginMenuBar())
        {
            m_MenuTree.Draw();
            ImGui::EndMenuBar();
        }
    }

    void MenuBar::RenderAsMain()
    {
        m_Height = ImGui::GetFrameHeight();

        if (ImGui::BeginMainMenuBar())
        {
            m_MenuTree.Draw();
            ImGui::EndMainMenuBar();
        }
    }

    float MenuBar::RenderInRect(const ImRect& rect)
    {
        m_Height = ImGui::GetFrameHeight();

        if (!BeginMenuBarInRect(rect))
            return rect.Min.x;

        m_MenuTree.Draw();
        const float menuEnd = ImGui::GetCurrentWindow()->DC.CursorPos.x;

        EndMenuBarInRect();

        return menuEnd;
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
