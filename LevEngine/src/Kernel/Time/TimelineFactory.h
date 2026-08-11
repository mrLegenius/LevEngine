#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class Timeline;
    struct TimelineParameters;

    class LEV_API TimelineFactory
    {
    public:
        static Ref<Timeline> CreateTimeline(const TimelineParameters&& timelineParameters);
    };
}


