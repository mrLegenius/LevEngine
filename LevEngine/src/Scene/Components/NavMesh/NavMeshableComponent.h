#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    enum SamplePolyAreas : int
    {
        SamplePolyAreaGround,
        SamplePolyAreaWater,
        SamplePolyAreaRoad,
        SamplePolyAreaDoor,
        SamplePolyAreaGrass,
        SamplePolyAreaJump
    };
    enum SamplePolyFlags : int
    {
        SamplePolyFlagsWalk		= 0x01,		// Ability to walk (ground, grass, road)
        SamplePolyFlagsSwim		= 0x02,		// Ability to swim (water).
        SamplePolyFlagsDoor		= 0x04,		// Ability to move through doors.
        SamplePolyFlagsJump		= 0x08,		// Ability to jump.
        SamplePolyFlagsDisabled	= 0x10,		// Disabled polygon
        SamplePolyFlagsAll		= 0xffff	// All abilities.
    };
    REGISTER_PARSE_TYPE(NavMeshableComponent);
	
    struct NavMeshableComponent
    {
        NavMeshableComponent();
        NavMeshableComponent(const NavMeshableComponent&) = default;
        
        void Function();
        void Build();
        int m_Field = 0;
        
        bool UseInNavMesh = true;
    };
}
