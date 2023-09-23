#include "levpch.h"
#include "TimelineFactory.h"

#include "Timeline.h"
#include "TimelineRunner.h"

LevEngine::Timeline* LevEngine::TimelineFactory::CreateTimeline()
{
    Timeline* timeline = new Timeline();
    TimelineRunner::AddTimeline(timeline);
    return timeline;
}
