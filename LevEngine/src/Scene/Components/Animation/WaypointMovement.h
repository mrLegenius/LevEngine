#pragma once
#include "DataTypes/Vector.h"

namespace LevEngine
{
    struct Transform;

    struct WaypointMovementComponent
    {
        Vector<Ref<Transform>> waypoints;
        
        WaypointMovementComponent();
    };   
}
