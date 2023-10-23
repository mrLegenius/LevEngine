#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include <entt/entt.hpp>

namespace LevEngine
{
    class AudioPlayer; 

    REGISTER_PARSE_TYPE(AudioSourceComponent);

    struct AudioSourceComponent
    {
        AudioSourceComponent();

        static void OnComponentConstruct(entt::registry& registry, entt::entity entity);
        static void OnComponentDestroy(entt::registry& registry, entt::entity entity);
        void Init();
        bool IsInitialized() const;
        void ResetInit();

        Ref<AudioPlayer> audioPlayer;

    private:
        bool m_IsInited{};
    };
}

