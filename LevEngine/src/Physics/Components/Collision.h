#pragma once
#include "Scene/Entity.h"

namespace LevEngine
{
    struct Collision
    {
        // The entity with which the collision occurred.
        Entity ContactEntity;
        // The position of the contact point between the shapes, in world space.
        Vector<Vector3> ContactPositions {};
        // The normal of the contacting surfaces at the contact point. The normal direction points from the second shape to the first shape.
        Vector<Vector3> ContactNormals {};
        // The impulse applied at the contact point, in world space. Divide by the simulation time step to get a force value.
        Vector<Vector3> ContactImpulses {};
        // The separation of the shapes at the contact point.  A negative separation denotes a penetration.
        Vector<float> ContactSeparations {};
        // The surface index of first shape at the contact point.  This is used to identify the surface material.
        Vector<int> ContactFirstColliderMaterialIndices {};
        // The surface index of second shape at the contact point.  This is used to identify the surface material.
        Vector<int> ContactSecondColliderMaterialIndices {};
    };
}
