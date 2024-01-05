#include "levpch.h"
#include "SkyboxRenderer.h"
#include "Assets/TextureAsset.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	SkyboxRendererComponent::SkyboxRendererComponent() = default;
	
	class SkyboxComponentSerializer final : public ComponentSerializer<SkyboxRendererComponent, SkyboxComponentSerializer>
	{
	protected:
		const char* GetKey() override { return "Skybox Renderer"; }

		void SerializeData(YAML::Emitter& out, const SkyboxRendererComponent& component) override
		{
			SerializeAsset(out, "Skybox", component.SkyboxTexture);
		}
		void DeserializeData(const YAML::Node& node, SkyboxRendererComponent& component) override
		{
			component.SkyboxTexture = DeserializeAsset<TextureAsset>(node["Skybox"]);
		}
	};
}
