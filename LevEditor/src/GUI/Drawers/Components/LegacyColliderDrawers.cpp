#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class LegacySphereColliderDrawer final : public ComponentDrawer<LegacySphereCollider, LegacySphereColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Legacy Sphere Collider"; }
        String GetAddMenuPath() const override { return "Legacy Physics"; }

        void DrawContent(LegacySphereCollider& component) override
        {
            EditorGUI::DrawFloatControl("Radius", component.radius);
            EditorGUI::DrawVector3Control("Offset", component.offset);
        }
    };

    class LegacyBoxColliderDrawer final : public ComponentDrawer<LegacyBoxCollider, LegacyBoxColliderDrawer>
    {
    protected:
        String GetLabel() const override { return "Legacy Box Collider"; }
        String GetAddMenuPath() const override { return "Legacy Physics"; }

        void DrawContent(LegacyBoxCollider& component) override
        {
            EditorGUI::DrawVector3Control("Extents", component.extents);
            EditorGUI::DrawVector3Control("Offset", component.offset);
        }
    };
}
