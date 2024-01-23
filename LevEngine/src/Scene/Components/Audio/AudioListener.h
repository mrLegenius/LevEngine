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

        static void OnConstruct(Entity entity);
        static void OnDestroy(Entity entity);
        void Init(Entity entity);
        bool IsInitialized() const;
        void ResetInit();

        Entity AttachedToEntity;

    private:
        bool m_IsInited{};
    };
}
