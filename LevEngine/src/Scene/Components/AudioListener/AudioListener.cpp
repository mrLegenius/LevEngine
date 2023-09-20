#include "levpch.h"
#include "AudioListener.h"

#include "Scene/Components/ComponentDrawer.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	AudioListenerComponent::AudioListenerComponent() = default;

	class AudioListenerComponentDrawer final : public ComponentDrawer<AudioListenerComponent, AudioListenerComponentDrawer>
	{
	protected:
		[[nodiscard]] String GetLabel() const override { return "Audio Listener"; }

		void DrawContent(AudioListenerComponent& component) override
		{
			auto& eventName = component.eventName;
			ImGui::InputText("Event name", eventName.data(), AudioListenerComponent::MaxStringLength);
		}
	};

	class AudioListenerSerializer final : public ComponentSerializer<AudioListenerComponent, AudioListenerSerializer>
	{
	protected:
		const char* GetKey() override { return "AudioListener"; } 

		void SerializeData(YAML::Emitter& out, const AudioListenerComponent& component) override
		{
			auto& eventName = component.eventName;
			out << YAML::Key << "AudioListener" << YAML::Value;
			
			out << YAML::BeginMap;
			out << YAML::Key << "AudioEventName" << YAML::Value << eventName.data();
			out << YAML::EndMap;
		}
		void DeserializeData(YAML::Node& node, AudioListenerComponent& component) override
		{
			auto audioListenerProps = node["AudioListener"];
			component.eventName = audioListenerProps["AudioEventName"].as<eastl::string>();
		}
	};
}
