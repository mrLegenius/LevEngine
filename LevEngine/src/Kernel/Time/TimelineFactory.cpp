#include "levpch.h"
#include "TimelineFactory.h"

#include "Timeline.h"
#include "TimelineParameters.h"
#include "TimelineRunner.h"

namespace LevEngine
{
    Ref<Timeline> TimelineFactory::CreateTimeline(const TimelineParameters&& timelineParameters)
    {
        Ref<Timeline> timeline = CreateRef<Timeline>(eastl::move(timelineParameters));
        TimelineRunner::AddTimeline(timeline);
        return timeline;
    }

    void TimelineFactory::RemoveTimeline(const Ref<Timeline>& timeline)
    {
        TimelineRunner::RemoveTimeline(timeline);
    }
}
