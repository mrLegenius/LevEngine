#pragma once
#include "ComponentDrawer.h"
#include "Transform.h"
#include "GUI/GUIUtils.h"
#include "Math/Math.h"

namespace LevEngine
{
	class TransformDrawer final : public ComponentDrawer<Transform>
	{
	public:
		~TransformDrawer() override = default;
	protected:
		void DrawContent(Transform& component) override
		{
			auto position = component.GetLocalPosition();
			GUIUtils::DrawVector3Control("Position", position);
			component.SetLocalPosition(position);

			Vector3 rotation = Math::ToDegrees(component.GetLocalRotation());
			GUIUtils::DrawVector3Control("Rotation", rotation);
			component.SetLocalRotationRadians(Math::ToRadians(rotation));

			auto scale = component.GetLocalScale();
			GUIUtils::DrawVector3Control("Scale", scale, 1.0f);
			component.SetLocalScale(scale);
		}

		std::string GetLabel() const override { return "Transform"; }
		bool IsRemovable() const override { return false; }
	};
}
