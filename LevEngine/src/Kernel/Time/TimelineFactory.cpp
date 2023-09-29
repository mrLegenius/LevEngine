#include "levpch.h"
#include "TimelineFactory.h"

#include "Timeline.h"
#include "TimelineParameters.h"
#include "TimelineRunner.h"

LevEngine::Timeline* LevEngine::TimelineFactory::CreateTimeline(Ref<TimelineParameters> timelineParameters)
{
    Timeline* timeline = new Timeline(timelineParameters);
    TimelineRunner::AddTimeline(timeline);
    return timeline;
}
