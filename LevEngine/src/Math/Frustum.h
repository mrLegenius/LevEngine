#pragma once
#include "Plane.h"

namespace LevEngine
{
struct Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};
}

