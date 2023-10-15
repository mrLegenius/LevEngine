#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class LevFmod;

    REGISTER_PARSE_TYPE(AudioListenerComponent);

    struct AudioListenerComponent
    {
        Entity entity;
        bool listenerLock = false;

        AudioListenerComponent();
        ~AudioListenerComponent();

    private:
        Ref<LevFmod> m_Fmod;
    };
}
