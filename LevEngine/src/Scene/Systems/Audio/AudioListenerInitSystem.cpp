#include "levpch.h"
#include "AudioListenerInitSystem.h"
#include "Scene/Components/Audio/AudioListener.h"

void LevEngine::AudioListenerInitSystem::Update(float deltaTime, entt::registry& registry)
{
    const auto view = registry.view<AudioListenerComponent>();

    for (const auto entity : view)
    {
        AudioListenerComponent& audioListenerComponent = view.get<AudioListenerComponent>(entity);

        if (!audioListenerComponent.IsInitialized())
        {
            auto entityWrapped = Entity(entt::handle{ registry, entity });
            audioListenerComponent.Init(entityWrapped);
        }
    }
}
