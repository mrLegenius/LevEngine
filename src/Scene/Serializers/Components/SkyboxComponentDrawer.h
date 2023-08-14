#pragma once
#include "ComponentSerializer.h"
#include "../SerializerUtils.h"

namespace LevEngine
{
	class SkyboxComponentSerializer final : public ComponentSerializer<SkyboxRendererComponent, SkyboxComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Transform"; }

		void SerializeData(YAML::Emitter& out, const SkyboxRendererComponent& component) override
		{
			if (component.skybox)
				out << YAML::Key << "Skybox" << YAML::Value << component.skybox->GetPath().string();
		}
		void DeserializeData(YAML::Node& node, SkyboxRendererComponent& component) override
		{
			const auto skybox = node["Skybox"];

			if (!skybox) return;

			std::filesystem::path assetPath = skybox.as<std::string>();
			const auto skyboxAsset = CreateRef<SkyboxAsset>(assetPath);

			if (skyboxAsset->Deserialize())
				component.skybox = skyboxAsset;
		}
	};
}
