#include "levpch.h"
#include "SkyboxRenderer.h"

#include "../ComponentSerializer.h"
#include "Assets/AssetDatabase.h"

namespace LevEngine
{
	SkyboxRendererComponent::SkyboxRendererComponent() = default;
	
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
