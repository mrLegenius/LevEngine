#include "levpch.h"
#include "AudioListener.h"

#include "FMOD/LevFmod.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Kernel/Application.h"

namespace LevEngine
{
	AudioListenerComponent::AudioListenerComponent()
	{
		entity = SceneManager::GetActiveScene()->GetEntityByComponent(this);
		m_Fmod = Application::Get().GetAudioSubsystem();
		m_Fmod->AddListener(this);
	}

	AudioListenerComponent::~AudioListenerComponent()
	{
		m_Fmod->RemoveListener(this);
	}

	class AudioListenerSerializer final : public ComponentSerializer<AudioListenerComponent, AudioListenerSerializer>
	{
	protected:
		const char* GetKey() override { return "AudioListener"; } 

		void SerializeData(YAML::Emitter& out, const AudioListenerComponent& component) override
		{
			
		}
		void DeserializeData(YAML::Node& node, AudioListenerComponent& component) override
		{
			
		}
	};
}
