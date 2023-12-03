#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    enum SamplePolyAreas
    {
        SAMPLE_POLYAREA_GROUND,
        SAMPLE_POLYAREA_WATER,
        SAMPLE_POLYAREA_ROAD,
        SAMPLE_POLYAREA_DOOR,
        SAMPLE_POLYAREA_GRASS,
        SAMPLE_POLYAREA_JUMP
    };
    enum SamplePolyFlags
    {
        SAMPLE_POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
        SAMPLE_POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
        SAMPLE_POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
        SAMPLE_POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
        SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
        SAMPLE_POLYFLAGS_ALL		= 0xffff	// All abilities.
    };
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
