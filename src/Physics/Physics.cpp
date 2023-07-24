#include "pch.h"
#include "Physics.h"
#include "Scene/Entity.h"

bool Physics::AABBTest(
    const Vector3& posA, const Vector3& posB,
    const Vector3& halfSizeA, const Vector3& halfSizeB)
{
    const Vector3 delta = posB - posA;
    const Vector3 totalSize = halfSizeA + halfSizeB;

    return abs(delta.x) < totalSize.x
        && abs(delta.y) < totalSize.y
        && abs(delta.z) < totalSize.z;
}

bool Physics::HasAABBIntersection(const BoxCollider& colliderA, const Transform& transformA,
	const BoxCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
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
	collisionInfo.SetContactPoint(Vector3(), Vector3(), bestAxis, penetration);
	return true;
}

bool Physics::HasSphereIntersection(const SphereCollider& colliderA, const Transform& transformA,
	const SphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
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

bool Physics::HasAABBSphereIntersection(const BoxCollider& colliderA, const Transform& transformA,
	const SphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
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

void Physics::HandleCollision(Transform& transformA, Rigidbody& rigidbodyA, Transform& transformB, Rigidbody& rigidbodyB, ContactPoint p)
{
	float totalMass = rigidbodyA.GetInverseMass() + rigidbodyB.GetInverseMass();

	// Separate them out using projection
	transformA.SetWorldPosition(transformA.GetWorldPosition()
		- p.normal
		* p.penetration
		* (rigidbodyA.GetInverseMass() / totalMass));

	transformB.SetWorldPosition(transformB.GetWorldPosition()
		+ p.normal
		* p.penetration
		* (rigidbodyB.GetInverseMass() / totalMass));

	// Impulse
	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;

	Vector3 angVelocityA = rigidbodyA.angularVelocity.Cross(relativeA);
	Vector3 angVelocityB = rigidbodyB.angularVelocity.Cross(relativeB);

	Vector3 fullVelocityA = rigidbodyA.velocity + angVelocityA;
	Vector3 fullVelocityB = rigidbodyB.velocity + angVelocityB;

	Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	float impulseForce = contactVelocity.Dot(p.normal);

	//Vector3 inertiaA = Vector3::Transform(relativeA.Cross(p.normal), rigidbodyA.GetInertiaTensor()).Cross(relativeA);
	Vector3 inertiaA = relativeA.Cross(p.normal).Cross(relativeA);
	//Vector3 inertiaB = Vector3::Transform(relativeB.Cross(p.normal), rigidbodyB.GetInertiaTensor()).Cross(relativeB);
	Vector3 inertiaB = relativeB.Cross(p.normal).Cross(relativeB);

	float angularEffect = (inertiaA + inertiaB).Dot(p.normal);

	float cRestitution = rigidbodyA.elasticity * rigidbodyB.elasticity; // disperse some kinetic energy

	float j = -(1.0f + cRestitution) * impulseForce / (totalMass + angularEffect);

	const Vector3 fullImpulse = p.normal * j;

	rigidbodyA.AddImpulse(-fullImpulse);
	rigidbodyB.AddImpulse(fullImpulse);

	rigidbodyA.AddAngularImpulse(relativeA.Cross(-fullImpulse));
	rigidbodyB.AddAngularImpulse(relativeB.Cross(fullImpulse));
}
