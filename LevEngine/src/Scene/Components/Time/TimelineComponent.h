#pragma once
#include "Scene/Components/Animation/WaypointMovement.h"

namespace LevEngine
{
    class Timeline;
}

struct TimelineComponent
{
    bool isLooping;
    double duration;
    LevEngine::Timeline* timeline;

    TimelineComponent();
    void Init();
    float GetElapsedTime() const;
};
