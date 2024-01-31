#include "levpch.h"
#include "AudioListener.h"
#include "Audio/Audio.h"

namespace LevEngine
{
	void AudioListenerComponent::OnConstruct(const Entity entity)
	{
		AudioListenerComponent& component = entity.GetComponent<AudioListenerComponent>();
		component.Owner = entity;
		Audio::AddListener(&component);
	}

	void AudioListenerComponent::OnDestroy(const Entity entity)
	{
		AudioListenerComponent& component = entity.GetComponent<AudioListenerComponent>();
		Audio::RemoveListener(&component);
	}
}
