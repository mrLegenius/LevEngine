#pragma once
#include "DataTypes/Vector.h"

namespace LevEngine
{
    class Timeline;

    class TimelineRunner
    {
    public:
        static void OnUpdate(float deltaTime);

    private:
        friend class TimelineFactory;
        static void AddTimeline(Timeline* timeline);
        
        static Vector<Timeline*> s_Timelines;
    };
}


