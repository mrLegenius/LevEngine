#pragma once
#include "ComponentDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine
{
	class CameraComponentDrawer final : public ComponentDrawer<CameraComponent>
	{
	protected:
		[[nodiscard]] std::string GetLabel() const override { return "Camera"; }

		void DrawContent(CameraComponent& component) override
		{
			auto& camera = component.camera;
			ImGui::Checkbox("Main", &component.isMain);
			ImGui::Checkbox("Fixed aspect ratio", &component.fixedAspectRatio);
			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(camera.GetProjectionType())];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					const bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float size = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Orthographic size", &size))
					camera.SetOrthographicSize(size);

				float nearClip = camera.GetOrthographicNear();
				if (ImGui::DragFloat("Near clip", &nearClip))
					camera.SetOrthographicNear(nearClip);

				float farClip = camera.GetOrthographicFar();
				if (ImGui::DragFloat("Far clip", &farClip))
					camera.SetOrthographicFar(farClip);
			}
			else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float fov = Math::ToDegrees(camera.GetFieldOfView());
				ImGui::DragFloat("Field of View", &fov);
				camera.SetFieldOfView(fov);

				float nearClip = camera.GetPerspectiveNear();
				ImGui::DragFloat("Near clip", &nearClip);
				camera.SetPerspectiveNear(nearClip);

				float farClip = camera.GetPerspectiveFar();
				ImGui::DragFloat("Far clip", &farClip);
				camera.SetPerspectiveFar(farClip);
			}
		}
	};
}
