#include "levpch.h"
#include "TimelineFactory.h"

#include "Timeline.h"
#include "TimelineParameters.h"
#include "TimelineRunner.h"

namespace LevEngine
{
    Ref<Timeline> TimelineFactory::CreateTimeline(const TimelineParameters&& timelineParameters)
    {
        return CreateRef<Timeline>(eastl::move(timelineParameters));
    }
}
