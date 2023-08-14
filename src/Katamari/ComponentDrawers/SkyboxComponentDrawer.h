#pragma once
#include "ComponentDrawer.h"
#include "Assets/SkyboxAsset.h"
#include "GUI/GUIUtils.h"
#include "Scene/Components/Components.h"

namespace LevEngine
{
	class SkyboxComponentDrawer final : public ComponentDrawer<SkyboxRendererComponent>
	{
	protected:
		std::string GetLabel() const override { return "Skybox Renderer"; }

		void DrawContent(SkyboxRendererComponent& component) override
		{
			GUIUtils::DrawAsset("Skybox", GUIUtils::IsAssetSkybox, [&component](const std::filesystem::path& assetPath)
				{
					const auto asset = CreateRef<SkyboxAsset>(assetPath);
					asset->Deserialize();
					component.texture = asset->GetTexture();
				});
		}
	};
}
