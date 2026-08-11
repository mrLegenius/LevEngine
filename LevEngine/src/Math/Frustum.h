#pragma once

#include "Kernel/Core.h"
#include "Plane.h"

namespace LevEngine
{
struct LEV_API Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};
}

