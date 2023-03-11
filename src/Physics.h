#pragma once
#include <algorithm>
#include <set>
#include <SimpleMath.h>
#include "GameObject.h"

using DirectX::SimpleMath::Vector3;
struct CollisionInfo;

static std::vector<std::shared_ptr<GameObject>> objects;
inline std::set<CollisionInfo> allCollisions;
int numCollisionFrames = 10;

struct ContactPoint
{
	Vector3 localA; // where did the collision occur ...
	Vector3 localB; // in the frame of each object !
	Vector3 normal;
	float penetration = 0;
};

struct CollisionInfo
{
    GameObject* a = nullptr;
    GameObject* b = nullptr;

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
		const size_t otherHash = (size_t)other.a + (size_t)other.b;
		const size_t thisHash = (size_t)a + (size_t)b;

        return thisHash < otherHash;
    }
};

inline bool AABBTest(
	const Vector3& posA, const Vector3& posB,
	const Vector3& halfSizeA, const Vector3& halfSizeB)
{
	const Vector3 delta = posB - posA;
	const Vector3 totalSize = halfSizeA + halfSizeB;

    return abs(delta.x) < totalSize.x
		&& abs(delta.y) < totalSize.y
		&& abs(delta.z) < totalSize.z;
}

inline bool HasAABBIntersection(const BoxCollider& colliderA, const Transform& transformA, const BoxCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
{
	const Vector3 boxAPos = transformA.GetWorldPosition() + colliderA.offset;
	const Vector3 boxBPos = transformB.GetWorldPosition() + colliderB.offset;
	const Vector3 boxASize = colliderA.extents;
	const Vector3 boxBSize = colliderB.extents;

	const bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

    if (!overlap) return false;

	static constexpr Vector3 faces[6] =
	{
		Vector3(-1, 0, 0), Vector3(1, 0, 0),
		Vector3(0, -1, 0), Vector3(0, 1, 0),
		Vector3(0, 0, -1), Vector3(0, 0, 1),
	};

	const Vector3 maxA = boxAPos + boxASize;
	const Vector3 minA = boxAPos - boxASize;
	const Vector3 maxB = boxBPos + boxBSize;
	const Vector3 minB = boxBPos - boxBSize;
	const float distances[6] =
	{
		(maxB.x - minA.x), // distance of box 'b' to 'left' of 'a'.
		(maxA.x - minB.x), // distance of box 'b' to 'right' of 'a'.
		(maxB.y - minA.y), // distance of box 'b' to 'bottom' of 'a'.
		(maxA.y - minB.y), // distance of box 'b' to 'top' of 'a'.
		(maxB.z - minA.z), // distance of box 'b' to 'far' of 'a'.
		(maxA.z - minB.z), // distance of box 'b' to 'near' of 'a'.
	};
	float penetration = FLT_MAX;
	Vector3 bestAxis;

	for (int i = 0; i < 6; i++)
	{
		if (distances[i] < penetration) 
        {
			penetration = distances[i];
			bestAxis = faces[i];
		}
	}
	collisionInfo.SetContactPoint(Vector3(), Vector3(),bestAxis, penetration);
	return true;
}

inline bool HasSphereIntersection(const SphereCollider& colliderA, const Transform& transformA, const SphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
{
    const Vector3 posA = transformA.GetWorldPosition() + colliderA.offset;
    const Vector3 posB = transformB.GetWorldPosition() + colliderB.offset;

	const float radiiSum = colliderA.radius + colliderB.radius;
	Vector3 delta = posB - posA;

	const float deltaLength = delta.Length();
    
	if (deltaLength > radiiSum) return false;

    const float penetration = radiiSum - deltaLength;
    delta.Normalize();
	const Vector3 normal = delta;
    const Vector3 localA = normal * colliderA.radius;
    const Vector3 localB = -normal * colliderB.radius;
	    
    collisionInfo.SetContactPoint(localA, localB, normal, penetration);
    return true;
}

inline bool HasAABBSphereIntersection(const BoxCollider& colliderA, const Transform& transformA, const SphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
{
    const Vector3 posA = transformA.GetWorldPosition() + colliderA.offset;
    const Vector3 posB = transformB.GetWorldPosition() + colliderB.offset;
    const Vector3 delta = posB - posA;

    const Vector3 boxSize = colliderA.extents;

    const auto closestPointOnBox = Vector3(
        std::clamp(delta.x, -boxSize.x, boxSize.x),
        std::clamp(delta.y, -boxSize.y, boxSize.y),
        std::clamp(delta.z, -boxSize.z, boxSize.z));

    Vector3 localPoint = delta - closestPointOnBox;
    const float distance = localPoint.Length();
    
    if (distance > colliderB.radius) return false;

    localPoint.Normalize();

    const Vector3 collisionNormal = localPoint;
    const float penetration = colliderB.radius - distance;

    const Vector3 localA = Vector3();
    const Vector3 localB = -collisionNormal * colliderB.radius;

    collisionInfo.SetContactPoint(localA, localB, collisionNormal, penetration);
    return true;
}

inline bool HasIntersection(const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b, CollisionInfo& collisionInfo)
{
    const auto colliderA = a->GetCollider();
    const auto colliderB = b->GetCollider();

    if (colliderA == nullptr || colliderB == nullptr) return false;

    collisionInfo.a = a.get();
    collisionInfo.b = b.get();

    auto transformA = a->GetTransform();
    auto transformB = b->GetTransform();

    if (colliderA->GetType() == ColliderType::Box && colliderB->GetType() == ColliderType::Box)
    {
        return HasAABBIntersection(static_cast<BoxCollider&>(*colliderA), *transformA, static_cast<BoxCollider&>(*colliderB), *transformB, collisionInfo);
    }
    else if (colliderA->GetType() == ColliderType::Sphere && colliderB->GetType() == ColliderType::Sphere)
    {
        return HasSphereIntersection(static_cast<SphereCollider&>(*colliderA), *transformA, static_cast<SphereCollider&>(*colliderB), *transformB, collisionInfo);
    }
    else if (colliderA->GetType() == ColliderType::Box && colliderB->GetType() == ColliderType::Sphere)
    {
        return HasAABBSphereIntersection(static_cast<BoxCollider&>(*colliderA), *transformA, static_cast<SphereCollider&>(*colliderB), *transformB, collisionInfo);
    }
    else if (colliderA->GetType() == ColliderType::Sphere && colliderB->GetType() == ColliderType::Box)
    {
        collisionInfo.a = b.get();
        collisionInfo.b = a.get();
        return HasAABBSphereIntersection(static_cast<BoxCollider&>(*colliderB), *transformB, static_cast<SphereCollider&>(*colliderA), *transformA, collisionInfo);
    }

    return false;
}

inline void HandleCollision(const CollisionInfo& collisionInfo)
{
    auto& p = collisionInfo.point;

    auto transformA = collisionInfo.a->GetTransform();
    auto transformB = collisionInfo.b->GetTransform();

    auto rigidbodyA = collisionInfo.a->GetRigidbody();
    auto rigidbodyB = collisionInfo.b->GetRigidbody();

    float totalMass = rigidbodyA->GetInverseMass() + rigidbodyB->GetInverseMass();

    // Separate them out using projection
    transformA->SetWorldPosition(transformA->GetWorldPosition()
        - p.normal 
          * p.penetration
          * (rigidbodyA->GetInverseMass() / totalMass));

    transformB->SetWorldPosition(transformB->GetWorldPosition()
        + p.normal 
          * p.penetration 
          * (rigidbodyB->GetInverseMass() / totalMass));

    // Impulse
    Vector3 relativeA = p.localA;
    Vector3 relativeB = p.localB;

    Vector3 angVelocityA = rigidbodyA->GetAngularVelocity().Cross(relativeA);
    Vector3 angVelocityB = rigidbodyB->GetAngularVelocity().Cross(relativeB);

    Vector3 fullVelocityA = rigidbodyA->GetLinearVelocity() + angVelocityA;
    Vector3 fullVelocityB = rigidbodyB->GetLinearVelocity() + angVelocityB;

    Vector3 contactVelocity = fullVelocityB - fullVelocityA;

    float impulseForce = contactVelocity.Dot(p.normal);

    //Vector3 inertiaA = Vector3::Transform(relativeA.Cross(p.normal), rigidbodyA->GetInertiaTensor()).Cross(relativeA);
    Vector3 inertiaA = relativeA.Cross(p.normal).Cross(relativeA);
    //Vector3 inertiaB = Vector3::Transform(relativeB.Cross(p.normal), rigidbodyB->GetInertiaTensor()).Cross(relativeB);
    Vector3 inertiaB = relativeB.Cross(p.normal).Cross(relativeB);

    float angularEffect = (inertiaA + inertiaB).Dot(p.normal);

    float cRestitution = rigidbodyA->elasticity * rigidbodyB->elasticity; // disperse some kinetic energy

    float j = -(1.0f + cRestitution) * impulseForce / (totalMass + angularEffect);

    const Vector3 fullImpulse = p.normal * j;

    rigidbodyA->AddImpulse(-fullImpulse);
    rigidbodyB->AddImpulse(fullImpulse);

    rigidbodyA->AddAngularImpulse(relativeA.Cross(-fullImpulse));
    rigidbodyB->AddAngularImpulse(relativeB.Cross(fullImpulse));
}

inline void BasicCollisionDetection()
{
    const auto first = objects.begin();
    const auto last = objects.end();

    for (auto i = first; i != last; ++i)
    {
	    if ((*i)->GetRigidbody() == nullptr || !(*i)->GetRigidbody()->enabled) continue;

        for (auto j = i + 1; j != last; ++j)
        {
	        if ((*j)->GetRigidbody() == nullptr || !(*j)->GetRigidbody()->enabled) continue;

            if (CollisionInfo info; HasIntersection(*i, *j, info))
            {
                HandleCollision(info);

                if (auto exist = allCollisions.find(info); exist == allCollisions.end())
                {
                    info.framesLeft = numCollisionFrames;
                    allCollisions.insert(info);
                }
                else
                {
                    exist->framesLeft = numCollisionFrames - 1;
                }
            }
        }
    }
}

inline void UpdateVelocities(const float deltaTime)
{
    for (const auto gameObject : objects)
        gameObject->GetRigidbody()->UpdateVelocity(deltaTime);
}

inline void UpdatePositions(const float deltaTime)
{
    for (const auto gameObject : objects)
        gameObject->GetRigidbody()->UpdatePosition(deltaTime);
}

inline void UpdateConstraints(const float deltaTime)
{
    for (const auto gameObject : objects)
        for (const auto& constraint : gameObject->GetConstraints())
            constraint->Update(deltaTime);
}

inline void ClearForces()
{
    for (const auto gameObject : objects)
        gameObject->GetRigidbody()->ClearForces();
}

inline void UpdateCollisionList()
{
    for (auto i = allCollisions.begin(); i != allCollisions.end(); )
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
    }
}

static float dTOffset = 0;

inline void UpdatePhysics(const float deltaTime, const std::vector<std::shared_ptr<GameObject>>& allObjects)
{
    objects = allObjects;
    constexpr float iterationDt = 1.0f / 120.0f; //Ideally we'll have 120 physics updates a second 
    dTOffset += deltaTime; //We accumulate time delta here - there might be remainders from previous frame!

    const int iterationCount = static_cast<int>(dTOffset / iterationDt); //And split it up here

    const float subDt = deltaTime / static_cast<float>(iterationCount);	//How many seconds per iteration do we get?

    UpdateVelocities(deltaTime);

    for (int i = 0; i < iterationCount; ++i) {
        /*if (useBroadPhase) { //TODO: Add optimization
            BroadPhase();
            NarrowPhase();
        }
        else {*/
        BasicCollisionDetection();
        //}

        //This is our simple iterative solver - 
        //we just run things multiple times, slowly moving things forward
        //and then rechecking that the constraints have been met

        constexpr int constraintIterationCount = 10;
        const float constraintDt = subDt / static_cast<float>(constraintIterationCount);

        for (int j = 0; j < constraintIterationCount; ++j) {
            UpdateConstraints(constraintDt);
            UpdatePositions(constraintDt);
        }
        dTOffset -= iterationDt;
    }
    ClearForces();	//Once we've finished with the forces, reset them to zero

    UpdateCollisionList(); //Remove any old collisions
}
