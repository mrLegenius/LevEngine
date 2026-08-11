#pragma once

#include "Kernel/Core.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class Entity;

    REGISTER_PARSE_TYPE(WaypointMovementComponent);
    
    struct LEV_API WaypointMovementComponent
    {
        WaypointMovementComponent();
        
        Vector<Entity> entities;
		float currentDisplacement{};
    }; 
}
