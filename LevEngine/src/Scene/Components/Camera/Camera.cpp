#include "pch.h"
#include "Camera.h"

#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine
{
	CameraComponent::CameraComponent() = default;

	class CameraComponentDrawer final : public ComponentDrawer<CameraComponent, CameraComponentDrawer>
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
				GUIUtils::DrawFloatControl("Orthographic size", BindGetter(&SceneCamera::GetOrthographicSize, &camera), BindSetter(&SceneCamera::SetOrthographicSize, &camera), 0.1f, 0, std::numeric_limits<float>::max());
				GUIUtils::DrawFloatControl("Near clip", BindGetter(&SceneCamera::GetOrthographicNear, &camera), BindSetter(&SceneCamera::SetOrthographicNear, &camera), 0.1f, 0.0001, camera.GetOrthographicFar() - 0.001f);
				GUIUtils::DrawFloatControl("Far clip", BindGetter(&SceneCamera::GetOrthographicFar, &camera), BindSetter(&SceneCamera::SetOrthographicFar, &camera), 0.1f, camera.GetOrthographicNear() + 0.001f, std::numeric_limits<float>::max());
			}
			else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				GUIUtils::DrawFloatControl("Field of View", BindGetter(&SceneCamera::GetFieldOfView, &camera), BindSetter(&SceneCamera::SetFieldOfView, &camera), 1.0f, 1, std::numeric_limits<float>::max());
				GUIUtils::DrawFloatControl("Near clip", BindGetter(&SceneCamera::GetPerspectiveNear, &camera), BindSetter(&SceneCamera::SetPerspectiveNear, &camera), 0.1f, 0.0001, camera.GetPerspectiveFar() - 0.001f);
				GUIUtils::DrawFloatControl("Far clip", BindGetter(&SceneCamera::GetPerspectiveFar, &camera), BindSetter(&SceneCamera::SetPerspectiveFar, &camera), 0.1f, camera.GetPerspectiveNear() + 0.001f, std::numeric_limits<float>::max());
			}
		}
	};

	class CameraSerializer final : public ComponentSerializer<CameraComponent, CameraSerializer>
	{
	protected:
		const char* GetKey() override { return "Camera"; } 

		void SerializeData(YAML::Emitter& out, const CameraComponent& component) override
		{
			auto& camera = component.camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.GetProjectionType());

			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetFieldOfView();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNear();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFar();

			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNear();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFar();
			out << YAML::EndMap;

			out << YAML::Key << "Main" << YAML::Value << component.isMain;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.fixedAspectRatio;
		}
		void DeserializeData(YAML::Node& node, CameraComponent& component) override
		{
			auto& camera = component.camera;
			auto cameraProps = node["Camera"];

			component.fixedAspectRatio = node["FixedAspectRatio"].as<bool>();
			component.isMain = node["Main"].as<bool>();
			camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraProps["ProjectionType"].as<int>()));

			camera.SetPerspective(
				cameraProps["PerspectiveFOV"].as<float>(),
				cameraProps["PerspectiveNear"].as<float>(),
				cameraProps["PerspectiveFar"].as<float>());

			camera.SetOrthographic(
				cameraProps["OrthographicSize"].as<float>(),
				cameraProps["OrthographicNear"].as<float>(),
				cameraProps["OrthographicFar"].as<float>());
		}
	};
}
