#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
	class CameraComponentDrawer final : public ComponentDrawer<CameraComponent, CameraComponentDrawer>
	{
	protected:
		[[nodiscard]] String GetLabel() const override { return "Camera"; }

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
				EditorGUI::DrawFloatControl("Orthographic size", BindGetter(&SceneCamera::GetOrthographicSize, &camera), BindSetter(&SceneCamera::SetOrthographicSize, &camera), 0.1f, 0.0f, std::numeric_limits<float>::max());
				EditorGUI::DrawFloatControl("Near clip", BindGetter(&SceneCamera::GetOrthographicNear, &camera), BindSetter(&SceneCamera::SetOrthographicNear, &camera), 0.1f, 0.0001f, camera.GetOrthographicFar() - 0.001f);
				EditorGUI::DrawFloatControl("Far clip", BindGetter(&SceneCamera::GetOrthographicFar, &camera), BindSetter(&SceneCamera::SetOrthographicFar, &camera), 0.1f, camera.GetOrthographicNear() + 0.001f, std::numeric_limits<float>::max());
			}
			else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				EditorGUI::DrawFloatControl("Field of View", BindGetter(&SceneCamera::GetFieldOfView, &camera), BindSetter(&SceneCamera::SetFieldOfView, &camera), 1.0f, 1.0f, std::numeric_limits<float>::max());
				EditorGUI::DrawFloatControl("Near clip", BindGetter(&SceneCamera::GetPerspectiveNear, &camera), BindSetter(&SceneCamera::SetPerspectiveNear, &camera), 0.1f, 0.0001f, camera.GetPerspectiveFar() - 0.001f);
				EditorGUI::DrawFloatControl("Far clip", BindGetter(&SceneCamera::GetPerspectiveFar, &camera), BindSetter(&SceneCamera::SetPerspectiveFar, &camera), 0.1f, camera.GetPerspectiveNear() + 0.001f, std::numeric_limits<float>::max());
			}
		}
	};
}
