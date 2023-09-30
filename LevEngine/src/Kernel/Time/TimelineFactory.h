#pragma once

class TimelineParameters;

namespace LevEngine
{
    class Timeline;

    class TimelineFactory
    {
    public:
        static Ref<Timeline> CreateTimeline(Ref<TimelineParameters> timelineParameters);
    };
}


