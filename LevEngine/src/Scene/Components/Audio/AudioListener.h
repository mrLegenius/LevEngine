#pragma once
#include "Scene/Entity.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Audio;

    REGISTER_PARSE_TYPE(AudioListenerComponent);

    struct AudioListenerComponent
    {
        AudioListenerComponent() = default;

        static void OnConstruct(Entity entity);
        static void OnDestroy(Entity entity);
        Entity Owner;
    };
}
