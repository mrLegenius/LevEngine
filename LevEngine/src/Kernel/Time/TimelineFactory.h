#pragma once

class TimelineParameters;

namespace LevEngine
{
    class Timeline;

    class TimelineFactory
    {
    public:
        static Timeline* CreateTimeline(Ref<TimelineParameters> timelineParameters);
    };
}


