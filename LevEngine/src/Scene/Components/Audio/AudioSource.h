#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class AudioPlayer; 

    REGISTER_PARSE_TYPE(AudioSourceComponent);

    struct AudioSourceComponent
    {
        AudioSourceComponent();

        static void OnConstruct(entt::registry& registry, entt::entity entity);
        void Init(Entity entity);
        bool IsInitialized() const;
        void ResetInit();

        Ref<AudioPlayer> Player;

    private:
        bool m_IsInited{};
    };
}

