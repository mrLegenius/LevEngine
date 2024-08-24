#pragma once

namespace LevEngine
{
    class Timeline;

    class TimelineRunner
    {
    public:
        static void OnUpdate(float deltaTime);
        static void AddTimeline(Ref<Timeline> timeline);
        static void RemoveTimeline(const Ref<Timeline>& timeline);
        
    private:
        static Vector<Ref<Timeline>> s_Timelines;
    };
}


