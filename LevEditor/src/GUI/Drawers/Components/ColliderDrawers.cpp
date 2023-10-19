#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class SphereColliderDrawer final : public ComponentDrawer<SphereCollider, SphereColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Sphere Collider"; }

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

        void DrawContent(BoxCollider& component) override
        {
            EditorGUI::DrawVector3Control("Extents", component.extents);
            EditorGUI::DrawVector3Control("Offset", component.offset);
        }
    };
}
