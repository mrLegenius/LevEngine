#pragma once
#include "Scene/Components/Transform/Transform.h"
#include "GUI/GUIUtils.h"
#include "Math/Math.h"
#include "ComponentDrawer.h"

namespace LevEngine
{
	class TransformDrawer final : public ComponentDrawer<Transform, TransformDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Transform"; }
		bool IsRemovable() const override { return false; }

		void DrawContent(Transform& component) override
		{
			auto position = component.GetLocalPosition();
			if (GUIUtils::DrawVector3Control("Position", position))
				component.SetLocalPosition(position);

			Vector3 rotation = Math::ToDegrees(component.GetLocalRotation());
			if (GUIUtils::DrawVector3Control("Rotation", rotation))
			component.SetLocalRotationRadians(Math::ToRadians(rotation));

			auto scale = component.GetLocalScale();
			if (GUIUtils::DrawVector3Control("Scale", scale, 1.0f))
				component.SetLocalScale(scale);
		}
	};
}
