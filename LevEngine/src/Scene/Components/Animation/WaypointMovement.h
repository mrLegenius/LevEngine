#pragma once
#include "DataTypes/Vector.h"

namespace LevEngine
{
    class Entity;

    struct WaypointMovementComponent
    {
        Vector<Entity> entities;
        float currentDisplacement;
        
        WaypointMovementComponent();
    };   
}
