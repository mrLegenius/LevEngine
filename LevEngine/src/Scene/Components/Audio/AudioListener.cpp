#include "levpch.h"
#include "AudioListener.h"

#include "Audio/Audio.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/ComponentSerializer.h"

namespace LevEngine
{
	void AudioListenerComponent::OnConstruct(entt::registry& registry, entt::entity entity)
	{
		AudioListenerComponent& component = registry.get<AudioListenerComponent>(entity);
		auto entityWrapped = Entity(entt::handle{ registry, entity });
		component.Init(entityWrapped);
	}

	void AudioListenerComponent::OnDestroy(entt::registry& registry, entt::entity entity)
	{
		AudioListenerComponent& component = registry.get<AudioListenerComponent>(entity);
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
		void DeserializeData(YAML::Node& node, AudioListenerComponent& component) override
		{
			component.ResetInit();
		}
	};
}
