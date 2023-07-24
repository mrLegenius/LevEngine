#pragma once
#include <SimpleMath.h>

#include "Components/Rigidbody.h"
#include "Components/Collider.h"
#include "Components/Transform.h"

namespace LevEngine
{
struct CollisionInfo;
class Entity;
using DirectX::SimpleMath::Vector3;

inline constexpr int NumCollisionFrames = 10;

struct ContactPoint
{
	Vector3 localA; // where did the collision occur ...
	Vector3 localB; // in the frame of each object !
	Vector3 normal;
	float penetration = 0;
};

struct CollisionInfo
{
    ContactPoint point;

    void SetContactPoint(const Vector3& localA, const Vector3& localB, const Vector3& normal, float p)
    {
        point.localA = localA;
        point.localB = localB;
        point.normal = normal;
        point.penetration = p;
    }
};

class Physics
{
public:
    static void HandleCollision(Transform& transformA, Rigidbody& rigidbodyA, Transform& transformB, Rigidbody& rigidbodyB, ContactPoint p);

    static bool AABBTest(
        const Vector3& posA, const Vector3& posB,
        const Vector3& halfSizeA, const Vector3& halfSizeB);

    static bool HasAABBIntersection(const BoxCollider& colliderA, const Transform& transformA, const BoxCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo);

    static bool HasSphereIntersection(const SphereCollider& colliderA, const Transform& transformA, const SphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo);

    static bool HasAABBSphereIntersection(const BoxCollider& colliderA, const Transform& transformA, const SphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo);

    static void UpdateConstraints(const float deltaTime)
    {
        /*for (const auto gameObject : objects)
            for (const auto& constraint : gameObject->GetConstraints())
                constraint->Update(deltaTime);*/
    }
};
}
