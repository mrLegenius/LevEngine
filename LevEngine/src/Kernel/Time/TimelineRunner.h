#pragma once

namespace LevEngine
{
    class Timeline;

    class TimelineRunner
    {
    public:
        static void OnUpdate(float deltaTime);

    private:
        friend class TimelineFactory;
        static void AddTimeline(Ref<Timeline> timeline);
        static void RemoveTimeline(const Ref<Timeline>& timeline);
        
        static Vector<Ref<Timeline>> s_Timelines;
    };
}


