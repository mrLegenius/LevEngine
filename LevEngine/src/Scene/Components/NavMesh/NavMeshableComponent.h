#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(NavMeshableComponent);
	
    struct NavMeshableComponent
    {
        NavMeshableComponent() = default;
        NavMeshableComponent(const NavMeshableComponent&) = default;

        void func();
        int field = 0;
        
        bool useInNavMesh = true;
    };
}
