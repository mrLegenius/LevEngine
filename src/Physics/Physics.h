#pragma once
#include <set>
#include <SimpleMath.h>

#include "Components/Rigidbody.h"
#include "Components/Collider.h"
#include "Components/Transform.h"

class Entity;
using DirectX::SimpleMath::Vector3;
struct CollisionInfo;

static std::set<CollisionInfo> allCollisions;


struct ContactPoint
{
	Vector3 localA; // where did the collision occur ...
	Vector3 localB; // in the frame of each object !
	Vector3 normal;
	float penetration = 0;
};

struct CollisionInfo
{
    Transform* transformA;
    Transform* transformB;

    Rigidbody* rigidbodyA;
    Rigidbody* rigidbodyB;

    ContactPoint point;
    mutable int framesLeft = 0;

    void SetContactPoint(const Vector3& localA, const Vector3& localB, const Vector3& normal, float p)
    {
        point.localA = localA;
        point.localB = localB;
        point.normal = normal;
        point.penetration = p;
    }

    bool operator<(const CollisionInfo& other) const
	{
		const size_t otherHash = (size_t)other.transformA + (size_t)other.transformB;
		const size_t thisHash = (size_t)transformA + (size_t)transformB;

        return thisHash < otherHash;
    }
};

class Physics
{
public:
    static bool HasIntersection(Entity a, Entity b, CollisionInfo& collisionInfo);
    static void HandleCollision(const CollisionInfo& collisionInfo);
private:

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

    static void UpdateCollisionList()
    {
        /*for (auto i = allCollisions.begin(); i != allCollisions.end(); )
        {
            if ((*i).framesLeft == numCollisionFrames)
            {
                i->a->OnCollisionBegin(i->b);
                i->b->OnCollisionBegin(i->a);
            }

            (*i).framesLeft = (*i).framesLeft - 1;

            if ((*i).framesLeft < 0)
            {
                i->a->OnCollisionEnd(i->b);
                i->b->OnCollisionEnd(i->a);
                i = allCollisions.erase(i);
            }
            else
            {
                ++i;
            }
        }*/
    }
};
