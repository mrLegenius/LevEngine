#pragma once
#include "DataTypes/Vector.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Entity;

    REGISTER_PARSE_TYPE(WaypointMovementComponent);
    
    struct WaypointMovementComponent
    {
        Vector<Entity> entities;
    };   
}
