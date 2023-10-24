#include "levpch.h"
#include "AudioListener.h"

#include "Audio/LevFmod.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	void AudioListenerComponent::OnComponentConstruct(entt::registry& registry, entt::entity entity)
	{
		AudioListenerComponent& component = registry.get<AudioListenerComponent>(entity);
		component.Init();
		
	}

	void AudioListenerComponent::OnComponentDestroy(entt::registry& registry, entt::entity entity)
	{
		AudioListenerComponent& component = registry.get<AudioListenerComponent>(entity);
		component.m_Fmod->RemoveListener(&component);
	}

	void AudioListenerComponent::Init()
	{
		if (IsInitialized())
		{
			return;
		}

		attachedToEntity = SceneManager::GetActiveScene()->GetEntityByComponent(this);
		m_Fmod = SceneManager::GetActiveScene()->GetAudioSubsystem();
		m_Fmod->AddListener(this);

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
		void DeserializeData(YAML::Node& node, AudioListenerComponent& component) override
		{
			component.ResetInit();
		}
	};
}
