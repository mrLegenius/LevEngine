#include "levpch.h"
#include "Camera.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	CameraComponent::CameraComponent() = default;

	class CameraSerializer final : public ComponentSerializer<CameraComponent, CameraSerializer>
	{
	protected:
		const char* GetKey() override { return "Camera"; } 

		void SerializeData(YAML::Emitter& out, const CameraComponent& component) override
		{
			auto& camera = component.Camera;
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

			out << YAML::Key << "Main" << YAML::Value << component.IsMain;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.FixedAspectRatio;
		}
		void DeserializeData(const YAML::Node& node, CameraComponent& component) override
		{
			auto& camera = component.Camera;
			auto cameraProps = node["Camera"];

			component.FixedAspectRatio = node["FixedAspectRatio"].as<bool>();
			component.IsMain = node["Main"].as<bool>();
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
