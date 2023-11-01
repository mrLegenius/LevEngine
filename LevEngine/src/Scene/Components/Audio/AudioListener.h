#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class Audio;

    REGISTER_PARSE_TYPE(AudioListenerComponent);

    struct AudioListenerComponent
    {
        AudioListenerComponent() = default;

        static void OnComponentConstruct(entt::registry& registry, entt::entity entity);
        static void OnComponentDestroy(entt::registry& registry, entt::entity entity);
        void Init(Entity entity);
        bool IsInitialized() const;
        void ResetInit();

        Entity attachedToEntity;

    private:
        bool m_IsInited{};
    };
}
