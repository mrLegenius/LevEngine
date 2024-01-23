#include "levpch.h"
#include "AudioListener.h"

#include "Audio/Audio.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	void AudioListenerComponent::OnConstruct(const Entity entity)
	{
		AudioListenerComponent& component = entity.GetComponent<AudioListenerComponent>();
		component.Init(entity);
	}

	void AudioListenerComponent::OnDestroy(const Entity entity)
	{
		AudioListenerComponent& component = entity.GetComponent<AudioListenerComponent>();
		Audio::RemoveListener(&component);
	}

	void AudioListenerComponent::Init(Entity entity)
	{
		if (IsInitialized()) return;

		AttachedToEntity = entity;
		Audio::AddListener(this);

		m_IsInited = true;
	}

	bool AudioListenerComponent::IsInitialized() const
	{
		return m_IsInited;
	}

	void AudioListenerComponent::ResetInit()
	{
		m_IsInited = false;
	}

	class AudioListenerSerializer final : public ComponentSerializer<AudioListenerComponent, AudioListenerSerializer>
	{
	protected:
		const char* GetKey() override { return "AudioListener"; } 

		void SerializeData(YAML::Emitter& out, const AudioListenerComponent& component) override
		{
			
		}
		void DeserializeData(const YAML::Node& node, AudioListenerComponent& component) override
		{
			component.ResetInit();
		}
	};
}
