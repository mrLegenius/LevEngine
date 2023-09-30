#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Timeline;
    
    REGISTER_PARSE_TYPE(TimelineComponent);

    struct TimelineComponent
    {
        bool isLooping;
        double duration;
        bool playOnInit;
        Timeline* timeline;

        void Init();
        
        [[nodiscard]] double GetDuration() const;
        void SetDuration(double duration);
        [[nodiscard]] double GetElapsedTime() const;
    };
}
