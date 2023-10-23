#include "levpch.h"
#include "AudioListener.h"

#include "Audio/LevFmod.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	AudioListenerComponent::AudioListenerComponent()
	{
		attachedToEntity = SceneManager::GetActiveScene()->GetEntityByComponent(this);
		m_Fmod = SceneManager::GetActiveScene()->GetAudioSubsystem();
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
