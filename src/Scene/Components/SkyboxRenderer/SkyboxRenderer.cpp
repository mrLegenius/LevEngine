#include "pch.h"
#include "SkyboxRenderer.h"

#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"

namespace LevEngine
{
	class SkyboxComponentDrawer final : public ComponentDrawer<SkyboxRendererComponent, SkyboxComponentDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Skybox Renderer"; }

		void DrawContent(SkyboxRendererComponent& component) override
		{
			GUIUtils::DrawAsset("Skybox", component.skybox,GUIUtils::IsAssetSkybox,
				[&component](const std::filesystem::path& assetPath)
				{
					const auto asset = CreateRef<SkyboxAsset>(assetPath);
					if (asset->Deserialize())
						component.skybox = asset;
				});
		}
	};

	class SkyboxComponentSerializer final : public ComponentSerializer<SkyboxRendererComponent, SkyboxComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Skybox Renderer"; }

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
