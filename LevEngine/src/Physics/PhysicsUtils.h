#pragma once
#include "Math/Vector3.h"
#include "physx/include/PxPhysicsAPI.h"
#include "Math/Quaternion.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    using namespace physx;
    
    struct PhysicsUtils
    {
        static PxVec3 FromVector3ToPxVec3(const Vector3& vector)
        {
            return PxVec3(vector.x, vector.y, vector.z);
        }
        static Vector3 FromPxVec3ToVector3(const PxVec3& vector)
        {
            return Vector3(vector.x, vector.y, vector.z);
        }
        static PxQuat FromQuaternionToPxQuat(const Quaternion& quaternion)
        {
            return PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
        }
        static Quaternion FromPxQuatToQuaternion(const PxQuat& quaternion)
        {
            return Quaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
        }
        static PxTransform FromTransformToPxTransform(const Transform& transform)
        {
            return PxTransform(FromVector3ToPxVec3(transform.GetWorldPosition()), FromQuaternionToPxQuat(transform.GetWorldRotation()));
        }
    };
}
