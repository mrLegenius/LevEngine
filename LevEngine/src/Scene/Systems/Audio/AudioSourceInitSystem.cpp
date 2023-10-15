#include "levpch.h"
#include "AudioSourceInitSystem.h"

#include "Scene/Components/Audio/AudioSource.h"

namespace LevEngine
{
    void AudioSourceInitSystem::Update(const float deltaTime, entt::registry& registry)
    {
        const auto view = registry.view<AudioSourceComponent>();

        for (const auto entity : view)
        {
            AudioSourceComponent& audioSourceComponent = view.get<AudioSourceComponent>(entity);

            if (!audioSourceComponent.IsInitialized())
            {
                audioSourceComponent.Init();
            }
        }
    }
}