#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Entity;
    class AudioPlayer; 

    REGISTER_PARSE_TYPE(AudioSourceComponent);

    struct AudioSourceComponent
    {
        static void OnConstruct(Entity entity);
        
        AudioSourceComponent();
        
        Ref<AudioPlayer> Player;
    };
}

