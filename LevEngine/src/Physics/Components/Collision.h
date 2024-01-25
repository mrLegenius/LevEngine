#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
    struct Collision
    {
        // The entity with which the collision occurred.
        Entity Entity;
        // The number of contacts for this collision
        int ContactCount = 0;
        // The position of the contact point between the shapes, in world space.
        Vector<Vector3> Points {};
        // The normal of the contacting surfaces at the contact point. The normal direction points from the second shape to the first shape.
        Vector<Vector3> Normals {};
        // The impulse applied at the contact point, in world space. Divide by the simulation time step to get a force value.
        Vector<Vector3> Impulses {};
        // The separation of the shapes at the contact point. A negative separation denotes a penetration.
        Vector<float> Separations {};
    };
}