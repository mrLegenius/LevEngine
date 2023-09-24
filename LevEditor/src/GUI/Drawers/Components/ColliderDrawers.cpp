#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class SphereColliderDrawer final : public ComponentDrawer<SphereCollider, SphereColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Sphere Collider"; }

        void DrawContent(SphereCollider& component) override
        {
            GUIUtils::DrawFloatControl("Radius", component.radius);
            GUIUtils::DrawVector3Control("Offset", component.offset);
        }
    };

    class BoxColliderDrawer final : public ComponentDrawer<BoxCollider, BoxColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Box Collider"; }

        void DrawContent(BoxCollider& component) override
        {
            GUIUtils::DrawVector3Control("Extents", component.extents);
            GUIUtils::DrawVector3Control("Offset", component.offset);
        }
    };
}
