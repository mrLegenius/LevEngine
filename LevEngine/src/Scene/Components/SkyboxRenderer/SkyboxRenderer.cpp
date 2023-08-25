#include "levpch.h"
#include "SkyboxRenderer.h"

#include "../ComponentDrawer.h"
#include "../ComponentSerializer.h"
#include "Assets/AssetDatabase.h"
#include "GUI/GUIUtils.h"

namespace LevEngine
{
	SkyboxRendererComponent::SkyboxRendererComponent() = default;

	class SkyboxComponentDrawer final : public ComponentDrawer<SkyboxRendererComponent, SkyboxComponentDrawer>
	{
	protected:
		std::string GetLabel() const override { return "Skybox Renderer"; }

		void DrawContent(SkyboxRendererComponent& component) override
		{
			GUIUtils::DrawAsset("Skybox", component.skybox, AssetDatabase::IsAssetSkybox,
				[&component](const std::filesystem::path& assetPath)
				{
					const auto asset = AssetDatabase::GetAsset<SkyboxAsset>(assetPath);
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
			SerializeAsset(out, "Skybox", component.skybox);
		}
		void DeserializeData(YAML::Node& node, SkyboxRendererComponent& component) override
		{
			component.skybox = DeserializeAsset<SkyboxAsset>(node["Skybox"]);
		}
	};
}
