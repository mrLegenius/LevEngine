#pragma once
#include "physx/include/PxPhysicsAPI.h"

#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
    struct PhysicsUtils
    {
        static physx::PxVec3 FromVector3ToPxVec3(const Vector3& vector)
        {
            return {vector.x, vector.y, vector.z};
        }
        static physx::PxExtendedVec3 FromVector3ToPxExtendedVec3(const Vector3& vector)
        {
            return {vector.x, vector.y, vector.z};
        }
        static Vector3 FromPxVec3ToVector3(const physx::PxVec3& vector)
        {
            return {vector.x, vector.y, vector.z};
        }
        static physx::PxQuat FromQuaternionToPxQuat(const Quaternion& quaternion)
        {
            return {quaternion.x, quaternion.y, quaternion.z, quaternion.w};
        }
        static Quaternion FromPxQuatToQuaternion(const physx::PxQuat& quaternion)
        {
            return {quaternion.x, quaternion.y, quaternion.z, quaternion.w};
        }
        static physx::PxTransform FromTransformToPxTransform(const Transform& transform)
        {
            return {FromVector3ToPxVec3(transform.GetWorldPosition()), FromQuaternionToPxQuat(transform.GetWorldRotation())};
        }
    };
}