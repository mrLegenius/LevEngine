#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class LevFmod;

    REGISTER_PARSE_TYPE(AudioListenerComponent);

    struct AudioListenerComponent
    {
        AudioListenerComponent() = default;

        static void OnComponentConstruct(entt::registry& registry, entt::entity entity);
        static void OnComponentDestroy(entt::registry& registry, entt::entity entity);
        void Init();
        bool IsInitialized() const;
        void ResetInit();

        Entity attachedToEntity;

        // When true, locks the listener in place, disabling internal 3D attribute updates.
        // 3D attributes can still be manually set with a set3DAttributes call.
        bool disableListener3DAttributes = false;

    private:
        Ref<LevFmod> m_Fmod;
        bool m_IsInited{};
    };
}
