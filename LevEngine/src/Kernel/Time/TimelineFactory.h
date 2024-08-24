#pragma once

namespace LevEngine
{
    class Timeline;
    struct TimelineParameters;

    class TimelineFactory
    {
    public:
        static Ref<Timeline> CreateTimeline(const TimelineParameters&& timelineParameters);
    };
}


