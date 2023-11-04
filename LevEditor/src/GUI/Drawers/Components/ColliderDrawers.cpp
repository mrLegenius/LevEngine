#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class SphereColliderDrawer final : public ComponentDrawer<SphereCollider, SphereColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Sphere Collider"; }
        String GetAddMenuPath() const override { return "Physics"; }

        void DrawContent(SphereCollider& component) override
        {
            EditorGUI::DrawFloatControl("Radius", component.radius);
            EditorGUI::DrawVector3Control("Offset", component.offset);
        }
    };

    class BoxColliderDrawer final : public ComponentDrawer<BoxCollider, BoxColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Box Collider"; }
        String GetAddMenuPath() const override { return "Physics"; }

        void DrawContent(BoxCollider& component) override
        {
            EditorGUI::DrawVector3Control("Extents", component.extents);
            EditorGUI::DrawVector3Control("Offset", component.offset);
        }
    };
}
