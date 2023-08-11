#pragma once
#include "GUI/GUIUtils.h"
#include "Scene/Components/ComponentDrawer.h"

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
				GUIUtils::DrawFloatControl("Orthographic size", BindGetter(&SceneCamera::GetOrthographicSize, &camera), BindSetter(&SceneCamera::SetOrthographicSize, &camera));
				GUIUtils::DrawFloatControl("Near clip", BindGetter(&SceneCamera::GetOrthographicNear, &camera), BindSetter(&SceneCamera::SetOrthographicNear, &camera));
				GUIUtils::DrawFloatControl("Far clip", BindGetter(&SceneCamera::GetOrthographicFar, &camera), BindSetter(&SceneCamera::SetOrthographicNear, &camera));
			}
			else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				GUIUtils::DrawFloatControl("Field of View", BindGetter(&SceneCamera::GetFieldOfView, &camera), BindSetter(&SceneCamera::SetFieldOfView, &camera));
				GUIUtils::DrawFloatControl("Near clip", BindGetter(&SceneCamera::GetPerspectiveNear, &camera), BindSetter(&SceneCamera::SetPerspectiveNear, &camera));
				GUIUtils::DrawFloatControl("Far clip", BindGetter(&SceneCamera::GetPerspectiveFar, &camera), BindSetter(&SceneCamera::SetPerspectiveFar, &camera));
			}
		}
	};
}
