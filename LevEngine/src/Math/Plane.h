#pragma once
#include "Vector3.h"

namespace LevEngine
{
struct Plane
{
    // unit vector
    Vector3 normal = { 0.f, 1.f, 0.f };

    // distance from origin to the nearest point in the plane
    float distance = 0.f;

    Plane() = default;

    Plane(const Vector3& p1, const Vector3& norm)
    {
        normal = norm;
        normal.Normalize();

        distance = normal.Dot(p1);
    }

    float GetSignedDistanceToPlane(const Vector3& point) const
    {
        return normal.Dot(point) - distance;
    }
};
}

