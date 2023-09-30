#include "levpch.h"
#include "TimelineFactory.h"

#include "Timeline.h"
#include "TimelineParameters.h"
#include "TimelineRunner.h"

namespace LevEngine
{
    Ref<Timeline> TimelineFactory::CreateTimeline(Ref<TimelineParameters> timelineParameters)
    {
        Ref<Timeline> timeline = CreateRef<Timeline>(timelineParameters);
        TimelineRunner::AddTimeline(timeline);
        return timeline;
    }

}