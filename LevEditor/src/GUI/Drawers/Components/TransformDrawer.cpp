#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class TransformDrawer final : public ComponentDrawer<Transform, TransformDrawer, -1000>
    {
    protected:
        String GetLabel() const override { return "Transform"; }
        bool IsRemovable() const override { return false; }

        void DrawContent(Transform& component) override
        {
            auto position = component.GetLocalPosition();
            if (GUIUtils::DrawVector3Control("Position", position))
                component.SetLocalPosition(position);

            Vector3 rotation = Math::ToDegrees(component.GetLocalRotation().ToEuler());
            if (GUIUtils::DrawVector3Control("Rotation", rotation))
                component.SetLocalRotation(Quaternion::CreateFromYawPitchRoll(Math::ToRadians(rotation)));

            auto scale = component.GetLocalScale();
            if (GUIUtils::DrawVector3Control("Scale", scale, 1.0f))
                component.SetLocalScale(scale);
        }
    };
}
