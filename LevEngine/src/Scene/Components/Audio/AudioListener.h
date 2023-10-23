#pragma once
#include "Scene/Components/TypeParseTraits.h"
#include "Scene/Entity.h"

namespace LevEngine
{
    class LevFmod;

    REGISTER_PARSE_TYPE(AudioListenerComponent);

    struct AudioListenerComponent
    {
        Entity attachedToEntity;

        // When true, locks the listener in place, disabling internal 3D attribute updates.
        // 3D attributes can still be manually set with a set3DAttributes call.
        bool disableListener3DAttributes = false;

        AudioListenerComponent();
        ~AudioListenerComponent();

    private:
        Ref<LevFmod> m_Fmod;
    };
}
